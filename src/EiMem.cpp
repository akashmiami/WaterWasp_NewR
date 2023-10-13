#include "freertos/FreeRTOS.h"
#include "AppConfig.h"
#include "Log.h"
#include "Memmgr.h"

#if ENABLE_EI_MEM

#define EI_MEM_CUSTOMIZE             1

void *ei_malloc(size_t size) {
    #if EI_MEM_CUSTOMIZE
    void *ptr =  memmgr_alloc(size);
    #else
    void *ptr = pvPortMalloc(size);
    printf("%08x - malloc %ld\r\n", (uint32_t)ptr, size);
    #endif /* EI_MEM_CUSTOMIZE */

    if (ptr == NULL) {
        LOG_INFO("--- Fail to malloc %ldbyte", size);
    }

    return ptr;
}

void *ei_calloc(size_t nitems, size_t size) {
    #if EI_MEM_CUSTOMIZE
    void *ptr = memmgr_alloc(nitems * size);
    #else
    void *ptr = pvPortMalloc(nitems * size);
    printf("%08x - calloc %ld\r\n", (uint32_t)ptr, nitems * size);
    #endif /* EI_MEM_CUSTOMIZE */


    if (ptr != NULL) {
        memset(ptr, 0, nitems * size);
    }
    else {
        LOG_ERR("~~~ Fail to alloc %ldbyte", size * nitems);
    }

    return ptr;
}

void ei_free(void *ptr) {
    #if EI_MEM_CUSTOMIZE
    memmgr_free(ptr);
    #else
    vPortFree(ptr);
    printf("%08x - Free\r\n", (uint32_t)ptr);
    #endif /* EI_MEM_CUSTOMIZE */

}

#endif /* ENABLE_EI_MEM */