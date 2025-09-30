#include "input_task.h"
#include "display_task.h"
#include "audio_task.h"
#include "call_state.h"

void kernel_init(void)
{
    DisplayTaskContext *display_ctx = NULL;
    AudioTaskContext *audio_ctx = NULL;
    CallStateContext *call_ctx = NULL;

    display_ctx = DisplayTask_Init();
    audio_ctx = AudioTask_Init();
    call_ctx = CallState_Init(display_ctx);
    InputTask_Init(display_ctx, audio_ctx);
}