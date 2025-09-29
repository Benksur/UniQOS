#ifndef INPUT_TASK_H_
#define INPUT_TASK_H_

#include "task_types.h"
#include "input.h"
#include "FreeRTOS.h"
#include <stdbool.h>

// Forward declaration
typedef struct DisplayTaskContext DisplayTaskContext;

// Function declarations
void input_task_main(void *pvParameters);
void input_task_init(void);
void input_task_set_display_context(DisplayTaskContext *ctx);

// Thread-safe input event functions
bool input_get_event(input_event_t *out_event, TickType_t timeout);
bool input_queue_available(void);

#endif // INPUT_TASK_H_