#include "input_task.h"
#include "display_task.h"

void kernel_init(void)
{
    DisplayTaskContext *display_ctx = NULL;

    // Initialize display task
    display_ctx = DisplayTask_Init();

    input_task_init();
    if (display_ctx)
    {
        input_task_set_display_context(display_ctx);
    }
}