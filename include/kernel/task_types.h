#ifndef TASK_TYPES_H_
#define TASK_TYPES_H_

#include <stdint.h>
#include "cmsis_os2.h"  // This includes osPriority_t

// Basic task status
typedef enum {
    TASK_OK = 0,
    TASK_ERROR,
    TASK_BUSY,
    TASK_TIMEOUT
} task_status_t;

// Use FreeRTOS priorities directly - no need to redefine
// Available priorities from cmsis_os2.h:
// osPriorityIdle = 1
// osPriorityLow = 8
// osPriorityBelowNormal = 16  
// osPriorityNormal = 24
// osPriorityAboveNormal = 32
// osPriorityHigh = 40
// osPriorityRealtime = 48

#endif // TASK_TYPES_H_