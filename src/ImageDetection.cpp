/*
 *  @file: ImageDetection.cpp
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

#if (DETECTION_METHOD == IMAGE_DETECTION)

#include "water-vision3_inferencing.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_camera.h"

#define EI_CAMERA_RAW_FRAME_BUFFER_COLS 320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS 240
#define EI_CAMERA_FRAME_BYTE_SIZE 3

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static TaskHandle_t DetectTaskHandle = NULL;
static bool debug_nn = false;
static bool is_initialised = false;
static int detectflag = 0;
static uint8_t *snapshot_buf; // points to the output of the capture

static float detect_tbl[RESULT_TABLE_SIZE_MAX + 1];

static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,

    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,

    // XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, // YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_QVGA,   // QQVGA-UXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 12, // 0-63 lower number means higher quality
    .fb_count = 1,      // if more than one, i2s runs in continuous mode. Use only with JPEG
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);

/**
 * @brief   Setup image sensor & start streaming
 *
 * @retval  false if initialisation failed
 */
bool ei_camera_init(void)
{
    if (is_initialised)
    {
        return true;
    }

#if (CAMERA_MODEL == CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    // initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        LOG_ERR("Camera init failed with error 0x%x", err);
        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);      // flip it back
        s->set_brightness(s, 1); // up the brightness just a bit
        s->set_saturation(s, 0); // lower the saturation
    }

#if (CAMERA_MODEL == CAMERA_MODEL_M5STACK_WIDE)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#elif (CAMERA_MODEL == CAMERA_MODEL_ESP_EYE)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
    s->set_awb_gain(s, 1);
#endif

    is_initialised = true;
    return true;
}

/**
 * @brief      Stop streaming of sensor data
 */
void ei_camera_deinit(void)
{
    // deinitialize the camera
    esp_err_t err = esp_camera_deinit();

    if (err != ESP_OK)
    {
        LOG_INFO("%s", "Camera deinit failed");
        return;
    }

    is_initialised = false;
    return;
}

/**
 * @brief      Capture, rescale and crop image
 *
 * @param[in]  img_width     width of output image
 * @param[in]  img_height    height of output image
 * @param[in]  out_buf       pointer to store output image, NULL may be used
 *                           if ei_camera_frame_buffer is to be used for capture and resize/cropping.
 *
 * @retval     false if not initialised, image captured, rescaled or cropped failed
 *
 */
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf)
{
    bool do_resize = false;

    if (!is_initialised)
    {
        LOG_ERR("%s", "ERR: Camera is not initialized");
        return false;
    }

    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb)
    {
        LOG_INFO("%s", "Camera capture failed");
        return false;
    }

    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);

    esp_camera_fb_return(fb);

    if (!converted)
    {
        LOG_INFO("%s", "Conversion failed");
        return false;
    }

    if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS) || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS))
    {
        do_resize = true;
    }

    if (do_resize)
    {
        ei::image::processing::crop_and_interpolate_rgb888(
            out_buf,
            EI_CAMERA_RAW_FRAME_BUFFER_COLS,
            EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
            out_buf,
            img_width,
            img_height);
    }

    return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    // we already have a RGB888 buffer, so recalculate offset into pixel index
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0)
    {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix + 2];

        // go to the next pixel
        out_ptr_ix++;
        pixel_ix += 3;
        pixels_left--;
    }

    return 0;
}

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
    for (int i = 0; i < det_tbl_max; i++)
    {
        if (detect_tbl[i] >= score)
        {
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
    for (int i = 0; i < det_tbl_max; i++)
    {
        avg += detect_tbl[i];
    }

    if (det_tbl_max == 0)
    {
        return 0;
    }
    else
    {
        return avg / det_tbl_max;
    }
}

void DetectTableClear()
{
    memset(detect_tbl, 0, sizeof(detect_tbl));
}

/**
 * @brief      Get data and run inferencing
 *
 * @param[in]  debug  Get debug info if true
 */
void DetectHandle(void *arg)
{
    static unsigned long last = millis();
    static int detcnt;
    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));

        if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false)
        {
            LOG_INFO("%s", "Failed to capture image");
        }

        // Run the classifier
        ei_impulse_result_t result = {0};

        EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
        if (err != EI_IMPULSE_OK)
        {
            LOG_INFO("ERR: Failed to run classifier (%d)", err);
            continue;
        }

        float score = result.classification[1].value;
        // LOG_DBG("Edge Score idx_01 %.2f, [%ld]", score, millis() - last);

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
        if (score > det_thres)
        {
            detcnt++;
            if (detcnt > det_tbl_max)
            {
                detcnt = det_tbl_max;
            }
        }
        else if (detcnt > 0)
        {
            detcnt--;
        }
#endif
        LOG_DBG("Predict => %.2f | %ld | thres: %.2f | cnt: %ld | %.2f | %ld | water %.1f | no water %.1f | bz %d |",
                score, millis() - last, det_thres, det_tcnt, avg_score, xPortGetFreeHeapSize(),
                result.classification[1].value, result.classification[0].value, M5BuzzCount());

        // LOG_DBG("Score | Water | N Water | DTime | Thres | Cnt | Avg Score | FreeHeap %s", "|");
        // LOG_DBG(" %.2f |  %.2f |  %.2f   |  %04d | %.2f  |  %02d |   %.2f    |  %07ld |",
        //         score, result.classification[1].value, result.classification[0].value,
        //         millis() - last, det_thres, det_tcnt, avg_score, xPortGetFreeHeapSize());

        if (detcnt >= det_tcnt)
        {
            detectflag = 1;
        }
        else
        {
            detectflag = 0;
        }
        last = millis();
    }
}

void DetectionInit(void)
{
    LOG_INFO("%s", "Edge Impulse Inferencing Demo");
    if (ei_camera_init() == false)
    {
        LOG_INFO("%s", "Failed to initialize Camera!");
    }
    else
    {
        LOG_INFO("%s", "Camera initialized");
    }

    snapshot_buf = (uint8_t *)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);
    if (snapshot_buf == nullptr)
    {
        LOG_INFO("%s", "ERR: Failed to allocate snapshot buffer!");
        return;
    }

    xTaskCreatePinnedToCore(
        DetectHandle,      /* Task function. */
        "DetectHandle",    /* name of task. */
        8192,              /* Stack size of task */
        NULL,              /* parameter of the task */
        1,                 /* priority of the task */
        &DetectTaskHandle, /* Task handle to keep track of created task */
        0);                /* pin task to core 0 */
}

void DetectionExit(void)
{
    if (DetectTaskHandle != NULL)
    {
        vTaskDelete(DetectTaskHandle);
        DetectTaskHandle = NULL;
    }

    ei_camera_deinit();
    free(snapshot_buf);
}

int IsWaterDetected(void)
{
    return detectflag;
}

#endif /* (DETECTION_METHOD == IMAGE_DETECTION) */
