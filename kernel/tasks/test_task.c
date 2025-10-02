#include "test_task.h"
#include "stm32_config.h"
#include <string.h>
#include <stdio.h>



// Main test task function
static void test_task_main(void *pvParameters)
{
    TestTaskContext *ctx = (TestTaskContext *)pvParameters;

    // Wait for other tasks to initialize
    osDelay(3000);

    // Run tests in sequence - less frequently to avoid interfering with input
    while (ctx->test_running)
    {
        
    }
}

// Public API implementation
TestTaskContext *TestTask_Init(CallStateContext *call_ctx, DisplayTaskContext *display_ctx)
{
    TestTaskContext ctx;
    ctx.call_ctx = call_ctx;
    ctx.display_ctx = display_ctx;
    ctx.test_running = true;
    ctx.test_counter = 0;

    // Create and start the test task
    osThreadAttr_t task_attr = {
        .name = "TestTask",
        .stack_size = TEST_TASK_STACK_SIZE,
        .priority = TEST_TASK_PRIORITY};

    osThreadId_t thread_id = osThreadNew(test_task_main, &ctx, &task_attr);
    if (!thread_id)
    {
        return NULL;
    }

    return &ctx;
}
