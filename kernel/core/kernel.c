#include "input_task.h"
#include "display_task.h"
#include "audio_task.h"

void kernel_init(void)
{
    DisplayTaskContext *display_ctx = NULL;
    AudioTaskContext *audio_ctx = NULL;

    display_ctx = DisplayTask_Init();
    audio_ctx = AudioTask_Init();
    InputTask_Init(display_ctx, audio_ctx);
}