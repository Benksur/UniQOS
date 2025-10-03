#ifndef TEST_TASK_H
#define TEST_TASK_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "task_types.h"
#include "call_state.h"
#include "display_task.h"

#define TEST_TASK_STACK_SIZE 1024
#define TEST_TASK_PRIORITY osPriorityLow

// Test task context
typedef struct
{
    CallStateContext *call_ctx;
    DisplayTaskContext *display_ctx;
    uint32_t test_counter;
    bool test_running;
} TestTaskContext;

// Test functions
void test_incoming_call_posting(TestTaskContext *ctx);
void test_call_state_transitions(TestTaskContext *ctx);
void test_display_commands(TestTaskContext *ctx);

// Public API
TestTaskContext *TestTask_Init(CallStateContext *call_ctx, DisplayTaskContext *display_ctx);

#endif // TEST_TASK_H
