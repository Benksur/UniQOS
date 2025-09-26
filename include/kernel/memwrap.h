#ifndef MEMWRAP_H
#define MEMWRAP_H

#if defined(USE_FREERTOS)
    #include "pv_port_malloc.h" // Or the correct FreeRTOS header
    #define mem_malloc(size) pvPortMalloc(size)
    #define mem_free(ptr)    vPortFree(ptr)
#else 
    #include <stdlib.h>
    #define mem_malloc(size) malloc(size)
    #define mem_free(ptr)    free(ptr)

#endif

#endif