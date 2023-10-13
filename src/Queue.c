#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "AppConfig.h"
#include "Utils.h"
#include "Log.h"
#include "Queue.h"

#if 0
#define Q_DBG printf
#else
#define Q_DBG(...)
#endif

/**
 * @brief data element structure
 *
 * Used as is hodlder the data
 */
typedef struct __xqueue_element {
    size_t dat_sz;              /**< element data size */
    uint8_t data[1];            /**< element data */
} element_t;


void *xqueue_init(int numbs_element) {
    QueueHandle_t *queue = (QueueHandle_t *)pvPortMalloc(sizeof(QueueHandle_t));
    if (queue == NULL) {
        return NULL;
    }
    /* Create queue */
    *queue = xQueueCreate(numbs_element, sizeof(element_t *));
    if (*queue == NULL ) {
        Q_DBG("%s", "queue was not created\r\n");
        return NULL;
    }

    return (void *)queue;
}

void xqueue_exit(void *queue) {
    vPortFree(queue);
}

esp_err_t xqueue_put(void *queue, uint8_t *data, size_t dat_sz) {
    element_t *ele;

    if (queue == NULL || data == NULL || dat_sz == 0) {
        return ESP_FAIL;
    }
    /* Allocated memory store the data */
    ele = (element_t *)pvPortMalloc(dat_sz + sizeof(element_t));
    if (ele == NULL) {
        Q_DBG("%s", "Not enough memory for xqueue element, drop!!!\r\n");
        return ESP_FAIL;
    }
    Q_DBG("Malloc %08x, size %d\r\n", (uint32_t)ele, dat_sz + sizeof(element_t));
    /* Copy data to holder */
    ele->dat_sz = dat_sz;
    memcpy(ele->data, data, dat_sz);
    /* Put holder to queue */
    BaseType_t result = xQueueSend(*(QueueHandle_t *)queue, (void *) &ele, pdMS_TO_TICKS(10));
    if (result != pdPASS) {
        if (result == errQUEUE_FULL) {
            element_t *tmp;
            /* Drop oldest package */
            xQueueReceive(*(QueueHandle_t *)queue, &tmp, pdMS_TO_TICKS(10));
            /* free the element */
            vPortFree(tmp);
            Q_DBG("Free   %08x, Drop oldest package\r\n", (uint32_t)tmp);
            /* Re-put new element */
            result = xQueueSend(*(QueueHandle_t *)queue, (void *) &ele, pdMS_TO_TICKS(10));
            if (result != pdPASS) {
                goto lb_error;
            }
        }
        else {
            goto lb_error;
        }
    }
    return ESP_OK;

lb_error:
    Q_DBG("%s", "Push to xqueue fail, drop!!!\r\n");
    vPortFree(ele);
    return ESP_FAIL;
}

esp_err_t xqueue_get(void *queue, uint8_t *data, size_t max_size, size_t *dat_sz) {
    esp_err_t err = ESP_FAIL;
    element_t *ele;

    if (queue == NULL || data == NULL || dat_sz == NULL || max_size == 0) {
        return err;
    }
    /* Get holder from queue */
    if (xQueueReceive(*(QueueHandle_t *)queue, &ele, pdMS_TO_TICKS(10)) == pdPASS) {
        if (max_size >= ele->dat_sz) {
            memcpy(data, ele->data,  ele->dat_sz);
            *dat_sz = ele->dat_sz;
            err = ESP_OK;
        }
        else {
            Q_DBG("%s", "data buff size not enough, drop!!!\r\n");
        }
        vPortFree(ele);
        Q_DBG("Free   %08x\r\n", (uint32_t)ele);
        return err;
    }

    return err;
}
