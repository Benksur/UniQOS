#include "input_task.h"
#include "display_task.h"

void kernel_init(void)
{
    input_task_init();
    display_task_init();
}