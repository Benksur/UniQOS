#include "input_task.h"
#include "display_task.h"
#include "audio_task.h"
#include "call_state.h"
#include "cellular_task.h"
#include "power_task.h"
#include "test_task.h"

void kernel_init(void)
{
    DisplayTaskContext *display_ctx = NULL;
    AudioTaskContext *audio_ctx = NULL;
    CallStateContext *call_ctx = NULL;
    CellularTaskContext *cellular_ctx = NULL;
    PowerTaskContext *power_ctx = NULL;
    TestTaskContext *test_ctx = NULL;

    // Initialize audio task first (no dependencies)
    audio_ctx = AudioTask_Init();

    // Initialize call state (needs display, but we'll set it later)
    call_ctx = CallState_Init(NULL);

    // Initialize display task with call context (cellular will be set later)
    display_ctx = DisplayTask_Init(call_ctx, NULL);

    // Initialize cellular task (needs display and call contexts)
    cellular_ctx = CellularTask_Init(display_ctx, call_ctx);

    // Initialize power task (needs display context)
    power_ctx = PowerTask_Init(display_ctx);

    // Now set the cross-references
    CallState_SetDisplayContext(call_ctx, display_ctx);
    DisplayTask_SetCellularContext(display_ctx, cellular_ctx);

    // input task should not have call ctx, this is just for testing
    InputTask_Init(display_ctx, audio_ctx, call_ctx);

}