#include "input_task.h"
#include "display_task.h"
#include "audio_task.h"
#include "call_state.h"
#include "test_task.h"

void kernel_init(void)
{
    DisplayTaskContext *display_ctx = NULL;
    AudioTaskContext *audio_ctx = NULL;
    CallStateContext *call_ctx = NULL;
    TestTaskContext *test_ctx = NULL;

    // Initialize audio task first (no dependencies)
    audio_ctx = AudioTask_Init();

    // Initialize call state first (needs display, but we'll set it later)
    call_ctx = CallState_Init(NULL);

    // Initialize display task with call context for callbacks
    display_ctx = DisplayTask_Init(call_ctx);

    // Now set the display context in call state
    CallState_SetDisplayContext(call_ctx, display_ctx);

    // Initialize input task with all contexts.
    // input task should not have call ctx, this is just for testing
    InputTask_Init(display_ctx, audio_ctx, call_ctx);

    // Initialize test task - DISABLED: Use manual test instead
    // test_ctx = TestTask_Init(call_ctx, display_ctx);
    // if (!test_ctx)
    // {
    //     // Test task failed to initialize, but continue with other tasks
    //     // This allows the system to run even if testing is not available
    // }
}