#include "input_task.h"
#include "display_task.h"

void kernel_init(void)
{
    DisplayTaskContext *display_ctx = NULL;

    // Initialize display task
    display_ctx = DisplayTask_Init();
    input_task_init(display_ctx);
}