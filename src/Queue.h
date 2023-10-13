/**
 * @file   xqueue.h 
 * @brief  implement FIFO queue base on freertos queue
 * Its operations are:
 *
 * operation            | description
 * ---------------------|---------------
 * xqueue_init()        | init the queue
 * xqueue_exit()        | destroy the queue
 * xqueue_put()         | put data to queue, the queue will be allocated new memory then copy to it, if the queue is full it will drop first element data
 * xqueue_get()         | get data from queue
 */

#ifndef __XQUEUE_H
#define __XQUEUE_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  init the queue
 * @param  numbs_element number element can hold in queue
 * @retval pointer to queue, or NULL when fail
 */
void *xqueue_init(int numbs_element);

/**
 * @brief  destroy the queue
 * @param  queue       point to queue
 * @retval None
 */
void xqueue_exit(void *queue);

/**
 * @brief  put data to queue
 * @param  queue       point to queue
 * @param  data        buffer hold data
 * @param  dat_sz      length of data
 * @retval ESP_OK when put success
 */
esp_err_t xqueue_put(void *queue, uint8_t *data, size_t dat_sz);

/**
 * @brief  get data from queue
 * @param  queue       point to queue
 * @param  data        buffer hold data
 * @param  max_size    sizeof buffer data
 * @param  dat_sz      length of data
 * @retval ESP_OK when send success
 */
esp_err_t xqueue_get(void *queue, uint8_t *data, size_t max_size, size_t *dat_sz);

#ifdef __cplusplus
}
#endif

#endif /* __XQUEUE_H */
