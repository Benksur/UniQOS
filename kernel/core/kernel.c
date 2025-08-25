#include "input_task.h"
#include "display_task.h"
#include "audio_task.h"

void kernel_init(void)
{
    AudioTaskContext* audio_ctx = NULL;
    // input_task_init();
    // display_task_init();
    audio_ctx = AudioTask_Init();
}