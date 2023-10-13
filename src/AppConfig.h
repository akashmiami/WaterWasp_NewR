#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_MODULE                                 "wasp"
#define LOG_LEVEL                                  4               /**< Log level */

#define BOARD_ESP32                                1
#define BOARD_M5STICKC                             2
#define BOARD_M5STICKCPLUS                         3
#define BOARD_M5ATOM                               4
#define BOARD_ESP32CAM                             5
#define BOARD_M5TIMER_CAM                          6

#define MIC_OTHER                                  1
#define MIC_INMP441                                2
#define MIC_SPM1423                                3

#define ENABLE_EI_MEM                              1
#define ENABLE_DEBUG                               1                /**< Debug mode */
#define ENABLE_WEB_LOG                             1

#define AUDIO_DETECTION                            1
#define IMAGE_DETECTION                            2

/* Enable Detection Type */
#define DETECTION_METHOD                           IMAGE_DETECTION

#define CAMERA_MODEL_ESP_EYE                       1
#define CAMERA_MODEL_AI_THINKER                    2

#ifndef BOARD_USING
#define BOARD_USING                                BOARD_M5ATOM //BOARD_ESP32  /*Enable Microphone Board Type*/
#endif /* BOARD_USING */

#if BOARD_USING == BOARD_ESP32
#define MIC_USING                                  MIC_INMP441
#elif BOARD_USING == BOARD_M5ATOM
#define MIC_USING                                  MIC_SPM1423
#else
#define MIC_USING                                  MIC_OTHER
#endif

/* Periph PINOUT */

#if BOARD_USING == BOARD_ESP32

#elif BOARD_USING == BOARD_M5STICKC

#define M5_IR                                      9
#define M5_LED                                     10
#define BUTTON_A_PIN                               37
#define BUTTON_B_PIN                               39

#elif BOARD_USING == BOARD_M5STICKCPLUS

#define M5_IR                                      9
#define M5_LED                                     10
#define M5_BUTTON_HOME                             37
#define M5_BUTTON_RST                              39
#define BUTTON_A_PIN                               26
#define BUTTON_B_PIN                               27
#define SPEAKER_PIN                                2
#define TONE_PIN_CHANNEL                           0

#elif BOARD_USING == BOARD_M5ATOM

#define M5_LED                                     27
#define NUM_LEDS                                   8
#define BUTTON_PIN                                 39

#define BUZZER_EXT_PIN                             26
#define BUZZER_FREQ                                4000
#define BUZZER_CHANNEL                             0 

#elif BOARD_USING == BOARD_ESP32CAM
#undef ENABLE_EI_MEM
#define ENABLE_EI_MEM                              0
#define CAMERA_MODEL                               CAMERA_MODEL_ESP_EYE
#define BUTTON_PIN                                 15
#define LEDR_PIN                                   21
#define LEDW_PIN                                   22

#if (CAMERA_MODEL == CAMERA_MODEL_ESP_EYE)
#define PWDN_GPIO_NUM                              -1
#define RESET_GPIO_NUM                             -1
#define XCLK_GPIO_NUM                              4
#define SIOD_GPIO_NUM                              18
#define SIOC_GPIO_NUM                              23

#define Y9_GPIO_NUM                                36
#define Y8_GPIO_NUM                                37
#define Y7_GPIO_NUM                                38
#define Y6_GPIO_NUM                                39
#define Y5_GPIO_NUM                                35
#define Y4_GPIO_NUM                                14
#define Y3_GPIO_NUM                                13
#define Y2_GPIO_NUM                                34
#define VSYNC_GPIO_NUM                             5
#define HREF_GPIO_NUM                              27
#define PCLK_GPIO_NUM                              25

#elif (CAMERA_MODEL == CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM                              32
#define RESET_GPIO_NUM                             -1
#define XCLK_GPIO_NUM                              0
#define SIOD_GPIO_NUM                              26
#define SIOC_GPIO_NUM                              27

#define Y9_GPIO_NUM                                35
#define Y8_GPIO_NUM                                34
#define Y7_GPIO_NUM                                39
#define Y6_GPIO_NUM                                36
#define Y5_GPIO_NUM                                21
#define Y4_GPIO_NUM                                19
#define Y3_GPIO_NUM                                18
#define Y2_GPIO_NUM                                5
#define VSYNC_GPIO_NUM                             25
#define HREF_GPIO_NUM                              23
#define PCLK_GPIO_NUM                              22

#else
#error "Camera model not selected"
#endif /*  (CAMERA_MODEL == CAMERA_MODEL_ESP_EYE) */

#elif BOARD_USING == BOARD_M5TIMER_CAM
#undef ENABLE_EI_MEM
#define ENABLE_EI_MEM                              0
#define CAMERA_MODEL                               CAMERA_MODEL_ESP_EYE
#define BUTTON_PIN                                 37
#define LEDB_PIN                                   2
 
#define BUZZER_EXT_PIN                             4
#define BUZZER_FREQ                                4000
#define BUZZER_CHANNEL                             0 

#define PWDN_GPIO_NUM                              -1
#define RESET_GPIO_NUM                             15
#define XCLK_GPIO_NUM                              27
#define SIOD_GPIO_NUM                              25
#define SIOC_GPIO_NUM                              23

#define Y9_GPIO_NUM                                19
#define Y8_GPIO_NUM                                36
#define Y7_GPIO_NUM                                18
#define Y6_GPIO_NUM                                39
#define Y5_GPIO_NUM                                5
#define Y4_GPIO_NUM                                34
#define Y3_GPIO_NUM                                35
#define Y2_GPIO_NUM                                32
#define VSYNC_GPIO_NUM                             22
#define HREF_GPIO_NUM                              26
#define PCLK_GPIO_NUM                              21
#endif

#if (BOARD_USING == BOARD_ESP32CAM)

#endif /* (BOARD_USING == BOARD_ESP32CAM) */

#define HOST_NAME                                  "WaterWasp"
#define SOFT_AP_SSID                               "WaterWasp"     /**< Default SoftAP ssid */
#define SOFT_AP_PASSWD                             "12345678"      /**< Default SoftAP password */

#if ENABLE_DEBUG
#define SOFT_AP_IP_ADDR                            10,10,0,1       /**< Default SoftAP ip addr */
#define SOFT_AP_GATEWAY_IP_ADDR                    10,10,0,1       /**< Default SoftAP gateway ip addr */
#define POLL_NEW_COMMAND_INTERVAL                  30000           /**< msec */
#else
#define SOFT_AP_IP_ADDR                            10,10,0,1       /**< Default SoftAP ip addr */
#define SOFT_AP_GATEWAY_IP_ADDR                    10,10,0,1       /**< Default SoftAP gateway ip addr */
#endif /* ENABLE_DEBUG */

#define MODEL_SVM                                  1
#define MODEL_CNN                                  2

#if (MIC_USING == MIC_INMP441)

/* For connecting INMP441 external mic*/
/*
#define MIC_PIN_BCK                                33
#define MIC_PIN_LRCK                               15
#define MIC_PIN_DATA                               -1
#define MIC_PIN_DATA_IN                            21
*/
/*========= ESP-EYE: ==============================*/
/*========= Also look for config .channel_format ===== */
#define MIC_PIN_BCK                                26
#define MIC_PIN_LRCK                               32
#define MIC_PIN_DATA                               -1
#define MIC_PIN_DATA_IN                            33
/*========= ESP-EYE: ==============================*/

#elif (MIC_USING == MIC_SPM1423)

/*========= M5Atom Echo: ==============================*/
#define MIC_PIN_BCK                                19
#define MIC_PIN_LRCK                               33
#define MIC_PIN_DATA                               22
#define MIC_PIN_DATA_IN                            23
/*========= AtomU Echo: ==============================*/

/*========= M5Atom USB: ==============================*/
/*
#define MIC_PIN_BCK                                -1
#define MIC_PIN_LRCK                               5
#define MIC_PIN_DATA                               -1
#define MIC_PIN_DATA_IN                            19
*/
/*========= M5Atom USB: ==============================*/

#else
#define MIC_PIN_CLK                                0               /**< Mic pin clk */
#define MIC_PIN_DATA                               34              /**< Mic pin data */
#endif

#ifndef POLL_NEW_COMMAND_INTERVAL
#define POLL_NEW_COMMAND_INTERVAL                  60000           /**< msec */
#endif

#ifdef BUZZER_EXT_PIN
#define ENABLE_EXTERNAL_BUZZER                     1
#endif

#define POLL_CONFIG_INTERVAL                       600000          /**< msec */
#define HEARTBEAT_INTERVAL                         120000          /**< msec */
#define SYNC_TIME_INTERVAL                         3600000         /**< msec */
#define RESYNC_TIME_WHEN_FAIL                      3000            /**< msec */
#define REQUEST_NORMAL                             3000            /**< msec */
#define REQUEST_CHECK_DEVICE_REGISTER              10000           /**< msec */
#define REQUEST_UPDATE_DATA                        20000           /**< msec */

#define CLOUD_BASE_URI                             "https://app.logiqfish.com"
#define FIRMWARE_BASE_URI                          "https://app.logiqfish.com/waterwasp"
#define FIRMWARE_FILE_NAME                         "firmware.bin"
#define FIRMWARE_VERSION                           "0.1"

#define MAX_RECORDS_STORE                          128             /**< numb records store in spiffs */
#define BUTTON_RESET_FACTORY_HOLD_TIME             10000

#define RESULT_TABLE_SIZE_MAX                      256
#define DEF_CONF_DET_MODEL                         MODEL_CNN
#define DEF_CONF_SCORE_LIMIT                       1000000

#if (DEF_CONF_DET_MODEL == MODEL_SVM)
#define DEF_CONF_DET_THRES                         85000
#elif (DEF_CONF_DET_MODEL == MODEL_CNN)
#define DEF_CONF_DET_THRES                         0.6
#endif

#define DEF_CONF_DET_TCNT                          6
#define DEF_CONF_DETCNT_MAX                        500
#define DEF_CONF_DETCNT_MIN                        0
#define DEF_CONF_DET_TABL_SIZE                     9
#define DEF_CONF_MIC_VOL_GAIN                      2
#define DEF_CONF_BUZZ_ENABLE                       1
#define DEF_CONF_AVG_GPM                           1.0f            /**< Gallons Per Minute */
#define DEF_CONF_GAIN_FACTOR                       30
#define DEF_CONF_MAX_GALLONS_WARNING               0.5f            /**< */
#define DEF_CONF_BEEP_WARNING_INTERVAL             2               /**< sec */
#define DEF_CONF_WATER_ALERT1_TIME                 60              /**< sec */
#define DEF_CONF_WATER_ALERT2_TIME                 120             /**< sec */
#define DEF_CONF_WATER_ALERT3_TIME                 180             /**< sec */
#define DEF_CONF_WATER_ALERT4_TIME                 240             /**< sec */
#define DEF_CONF_WATER_KILLER_TIME                 300             /**< sec */
#define DEF_CONF_WATER_KILLER_RESET_TIME           30              /**< sec */
#define DEF_CONF_WATER_AVG_RESET_TIME              20              /**< sec */
#define DEF_CONF_FILTER_ENABLE                     1

#ifdef __cplusplus
}
#endif

#endif /* __APP_CONFIG_H */
