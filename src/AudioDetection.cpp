/*
 *  @file: AudioDetection.cpp
 *
 *  Created on:
 */

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "Arduino.h"
#include "AppConfig.h"
#include "Log.h"
#include "Utils.h"
#include "Detection.h"
#include "M5Peripheral.h"

#if (DETECTION_METHOD == AUDIO_DETECTION)

#include "filters.h"

// If your target is limited in memory remove this macro to save 10K RAM
#define EIDSP_QUANTIZE_FILTERBANK                  0
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW      3

#include "new-enzus-project-1_inferencing.h"

/******************************************************************************/
/*                              TYPES and DEFINITIONS                         */
/******************************************************************************/


#define DETECTION_CONTINUOUS                       1
#define I2S_BUFFER_LEN                             1024
#define SAMPLING_FREQUENCY                         16000

/** Audio buffers, pointers and selectors */
typedef struct {
    int16_t *buffers[2];
    uint8_t buf_ready;
    uint8_t buf_select;
    uint32_t buf_count;
    uint32_t n_samples;
} inference_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static int detectflag = 0;
static uint8_t i2s_buffer[I2S_BUFFER_LEN];
static float detect_tbl[RESULT_TABLE_SIZE_MAX + 1];

unsigned int sampling_period_us;

static inference_t inference;
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

const float cutoff_freq   = 10.0;   //Cutoff frequency in Hz
const float sampling_time = 0.001;  //Sampling time in seconds.
Filter fhp(cutoff_freq, sampling_time, IIR::ORDER::OD2, IIR::TYPE::HIGHPASS);

static TaskHandle_t MicRecordTaskHandle = NULL;
static TaskHandle_t DetectTaskHandle = NULL;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/


void DetectTablePush(int mode, float score)
{
    static int idx = 0;
    int det_tbl_max = ConfigGetInt(CONF_DET_TABL_SIZE);

    det_tbl_max = (det_tbl_max > RESULT_TABLE_SIZE_MAX) ? RESULT_TABLE_SIZE_MAX : det_tbl_max;
    detect_tbl[idx] = score;
    idx = (idx >= det_tbl_max) ? 0 : idx + 1;
}

int DetectTableCountHigherScore(float score)
{
    int count = 0;
    int det_tbl_max = ConfigGetInt(CONF_DET_TABL_SIZE);

    det_tbl_max = (det_tbl_max > RESULT_TABLE_SIZE_MAX) ? RESULT_TABLE_SIZE_MAX : det_tbl_max;
    for (int i = 0; i < det_tbl_max; i++) {
        if (detect_tbl[i] >= score) {
            count++;
        }
    }

    return count;
}

float DetectTableAverageScore()
{
    double avg = 0;
    int det_tbl_max = ConfigGetInt(CONF_DET_TABL_SIZE);

    det_tbl_max = (det_tbl_max > RESULT_TABLE_SIZE_MAX) ? RESULT_TABLE_SIZE_MAX : det_tbl_max;
    for (int i = 0; i < det_tbl_max; i++) {
        avg += detect_tbl[i];
    }

    if (det_tbl_max == 0) {
        return 0;
    }
    else {
        return avg / det_tbl_max;
    }
}

void DetectTableClear()
{
    memset(detect_tbl, 0, sizeof(detect_tbl));
}

/**
 * @fn     I2sInit
 * @brief  init i2s hardware
 */
void I2sInit()
{
    i2s_config_t i2s_config = 
    #if (MIC_USING == MIC_INMP441)
    {
        /*
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate =  SAMPLING_FREQUENCY,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        //.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        //.tx_desc_auto_clear = false,
        //.fixed_mclk = 0
        */

        /*Esp-Eye*/
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLING_FREQUENCY,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT, //<-- Esp-Eye
        //.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0

    };
    #elif (MIC_USING == MIC_SPM1423)
    {
        /*
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),// Set the I2S operating mode.  设置I2S工作模式
        .sample_rate = SAMPLING_FREQUENCY,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 128,
        */

        /*As suggested by M5 Stack Support*/
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
        .sample_rate          = SAMPLING_FREQUENCY,
        .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format       = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL2,
        .dma_buf_count        = 8,
        .dma_buf_len          = 200,
        .use_apll             = 0,


    };
    #else
    {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
        .sample_rate =  SAMPLING_FREQUENCY,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 128,
    };
    #endif

    #if (MIC_USING == MIC_INMP441)
    i2s_pin_config_t pin_config =
    {
        .bck_io_num   = MIC_PIN_BCK,
        .ws_io_num    = MIC_PIN_LRCK,
        .data_out_num = MIC_PIN_DATA,
        .data_in_num  = MIC_PIN_DATA_IN,
    };
    #elif (MIC_USING == MIC_SPM1423)
    i2s_pin_config_t pin_config;
    pin_config.bck_io_num = MIC_PIN_BCK;
    pin_config.ws_io_num = MIC_PIN_LRCK;
    pin_config.data_out_num = MIC_PIN_DATA;
    pin_config.data_in_num = MIC_PIN_DATA_IN;
    pin_config.mck_io_num = GPIO_NUM_0;
    #else
    i2s_pin_config_t pin_config;
    pin_config.bck_io_num   = I2S_PIN_NO_CHANGE;
    pin_config.ws_io_num    = MIC_PIN_CLK;
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    pin_config.data_in_num  = MIC_PIN_DATA;
    pin_config.mck_io_num = GPIO_NUM_0;
    #endif /* */

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);

    #if (MIC_USING == MIC_INMP441)

    #else
    i2s_set_clk(I2S_NUM_0, SAMPLING_FREQUENCY, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
    #endif
}

/**
 * Get raw audio signal data
 */
static int MicGetData(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);

    return 0;
}


void EiImpulseDetect()
{
    static unsigned long last = millis();
    static int detcnt;
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
    signal.get_data = &MicGetData;
    ei_impulse_result_t result = { 0 };

    #if DETECTION_CONTINUOUS
    EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
    #else
    EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debug_nn);
    #endif
    if (r != EI_IMPULSE_OK) {
        LOG_ERR("ERR: Failed to run classifier (%d)\n", r);
        return;
    }

    float score = result.classification[1].value;
    //LOG_DBG("Edge Score idx_01 %.2f, [%ld]", score, millis() - last);

    DetectTablePush(MODEL_CNN, score);
    double det_thres = ConfigGetDouble(CONF_DET_THRES);
    int det_tcnt = ConfigGetInt(CONF_DET_TCNT);
    int det_tbl_max = ConfigGetInt(CONF_DET_TABL_SIZE);
    float avg_score = DetectTableAverageScore();

    det_tbl_max = (det_tbl_max > RESULT_TABLE_SIZE_MAX) ? RESULT_TABLE_SIZE_MAX : det_tbl_max;
    det_tcnt = (det_tcnt > det_tbl_max) ? det_tbl_max : det_tcnt;
    #if 1
    detcnt = DetectTableCountHigherScore(det_thres);
    #else
    if (score > det_thres) {
        detcnt++;
        if (detcnt > det_tbl_max) {
            detcnt = det_tbl_max;
        }
    } 
    else if (detcnt > 0) {
      detcnt--;
    }
    #endif
    LOG_DBG("Predict => %.2f | %ld | thres: %.2f | cnt: %ld | %.2f | %ld | bz %d |", 
             score, millis() - last, det_thres, det_tcnt, avg_score, xPortGetFreeHeapSize(), M5BuzzCount());
    
    if (detcnt >= det_tcnt) {
        detectflag = 1;
    }
    else {
        detectflag = 0;
    }
    last = millis();
}

/**
 * @fn     IsWaterDetected
 * @brief
 * @return 1 When detected water
 *         0 Other
 */
int IsWaterDetected()
{
    return detectflag;
}

/**
 * @fn     MicRecordGetData
 * @return pointer to mic data 
 */
int16_t *MicRecordGetData(void)
{
    return inference.buffers[inference.buf_select];
}

/**
 * @fn     MicRecordTask
 * @brief  Get data from mic and push to buffer
 * @param[in]  arg
 */
void MicRecordTask (void *arg)
{
    #if MIC_USING == MIC_INMP441
    int32_t *data = (int32_t *)i2s_buffer;
    #else
    int16_t *data = (int16_t *)i2s_buffer;
    #endif

    size_t bytesread;
    int16_t val;
    uint8_t is_negative = 0;
    int vol_gain = DEF_CONF_MIC_VOL_GAIN;
    int filter_en = DEF_CONF_FILTER_ENABLE;
    uint32_t timeread = portMAX_DELAY;
    uint32_t last_ready = 0;
    delay(500);
    while (1) {
        // vTaskDelay(pdMS_TO_TICKS(50));
        // if (M5BuzzIsOn()) {
        //     continue;
        // }

        // vol_gain = ConfigGetInt(CONF_MIC_VOL_GAIN);
        filter_en = ConfigGetInt(CONF_FILTER_EN);
        i2s_read(I2S_NUM_0, (char *) i2s_buffer, sizeof(i2s_buffer), &bytesread, timeread);
        if (bytesread > 0) {
            #if (MIC_USING == MIC_INMP441)
            for (int i = 0; i < (bytesread >> 2); i++) {
                val = (int16_t)(data[i] >> 16);
            #else
            for (int i = 0; i < (bytesread >> 1); i++) {
                val = data[i];
            #endif

                // if (val & 0x8000) {
                //     val = ~val + 1;
                //     is_negative = 1;
                // }

                // val = val * vol_gain;
                // if (is_negative) {
                //     val = ~val + 1;
                //     is_negative = 0;
                // }
                // Serial.println(val);
                if (!filter_en) {
                    inference.buffers[inference.buf_select][inference.buf_count++] = val;
                }
                else {
                    inference.buffers[inference.buf_select][inference.buf_count++] = fhp.filterIn(val);
                }

                if (inference.buf_count >= inference.n_samples) {
                    inference.buf_select ^= 1;
                    inference.buf_count = 0;
                    inference.buf_ready = 1;
                    // uint32_t current = millis();
                    // printf("=== %ld\r\n", current - last_ready);
                    // last_ready = current;
                }
            }
        }
    }
}

/**
 * @fn     DetectHandle
 * @brief  Run detect water when buffer data ready
 * @param[in]  arg
 */
void DetectHandle(void *arg) {
    static int old_mode = -1;

    while (1) {
        if (inference.buf_ready) {
            inference.buf_ready = 0;
            EiImpulseDetect();
        }

        vTaskDelay(pdMS_TO_TICKS(10)); //TEMP change to 10
    }
}


/**
 * @fn     DetectionInit
 * @brief
 */
void DetectionInit(void) {
    uint16_t n_samples = EI_CLASSIFIER_SLICE_SIZE;

    inference.buf_count = 0;
    inference.buf_ready = 0;
    inference.buf_select = 0;
    inference.n_samples = n_samples;
    inference.buffers[0] = (int16_t *)malloc(n_samples * sizeof(int16_t));
    inference.buffers[1] = (int16_t *)malloc(n_samples * sizeof(int16_t));
    if (inference.buffers[0] == NULL || inference.buffers[1] == NULL) {
        LOG_ERR("%s", "NO mem for ei impulse buffer");
        return;
    }

    LOG_INFO("%s", "Inferencing settings:");
    LOG_INFO("\tInterval: %.2f ms.", (float)EI_CLASSIFIER_INTERVAL_MS);
    LOG_INFO("\tFrame size: %d", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    LOG_INFO("\tSample length: %d ms.", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
    LOG_INFO("\tNo. of classes: %d", sizeof(ei_classifier_inferencing_categories) /
                sizeof(ei_classifier_inferencing_categories[0]));
    LOG_INFO("EI_CLASSIFIER_SLICE_SIZE : %d", EI_CLASSIFIER_SLICE_SIZE);
  
    run_classifier_init();
    sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
    //
    I2sInit();
    //
    xTaskCreatePinnedToCore(
            MicRecordTask,            /* Task function. */
            "MicRecord",              /* name of task. */
            1536,                     /* Stack size of task */
            NULL,                     /* parameter of the task */
            configMAX_PRIORITIES - 2, /* priority of the task */
            &MicRecordTaskHandle,     /* Task handle to keep track of created task */
            0);                       /* pin task to core 0 */

    xTaskCreatePinnedToCore(
            DetectHandle,             /* Task function. */
            "DetectHandle",           /* name of task. */
            3072,                     /* Stack size of task */
            NULL,                     /* parameter of the task */
            1,                        /* priority of the task */
            &DetectTaskHandle,        /* Task handle to keep track of created task */
            0);                       /* pin task to core 0 */
}

void DetectionExit(void) 
{
    if (DetectTaskHandle != NULL) {
        vTaskDelete(DetectTaskHandle);
        DetectTaskHandle = NULL;
    }
    
    if (MicRecordTaskHandle != NULL) {
        vTaskDelete(MicRecordTaskHandle);
        MicRecordTaskHandle = NULL;
    }

    if (inference.buffers[0]) {
        free(inference.buffers[0]);
        inference.buffers[0] = 0;
    }

    if (inference.buffers[1]) {
        free(inference.buffers[1]);
        inference.buffers[1] = 0;
    }
}

#endif /* DETECTION_METHOD == AUDIO_DETECTION */
