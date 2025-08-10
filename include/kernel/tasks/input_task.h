#ifndef INPUT_TASK_H_
#define INPUT_TASK_H_

#include "task_types.h"
#include "ui/input.h"
#include "FreeRTOS.h"
#include <stdbool.h>

// Function declarations
void input_task_main(void *pvParameters);
void input_task_init(void);

// Thread-safe input event functions
bool input_get_event(input_event_t* out_event, TickType_t timeout);
bool input_queue_available(void);

#endif // INPUT_TASK_H_ 