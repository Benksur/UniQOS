#ifndef CALL_STATE_H
#define CALL_STATE_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "task_types.h"
#include "queue.h"
#include "event_groups.h"
#include "display_task.h"
#include "drv2603.h"
#include "stm32_config.h"

#define CALL_STATE_TASK_STACK_SIZE 1024
#define CALL_STATE_TASK_PRIORITY osPriorityAboveNormal

// Call state definitions
typedef enum
{
    CALL_STATE_IDLE,
    CALL_STATE_RINGING,
    CALL_STATE_DIALLING,
    CALL_STATE_ACTIVE,
    CALL_STATE_ENDING
} CallState;

// Call commands
typedef enum
{
    CALL_CMD_INCOMING_CALL,
    CALL_CMD_ANSWER_CALL,
    CALL_CMD_DIALLING,
    CALL_CMD_HANGUP_CALL,
} CallCommand;

// Event group bits for coordination
#define CALL_EVENT_INCOMING (1 << 0) // Incoming call detected (controls vibration)
#define CALL_EVENT_ANSWER (1 << 1)   // Call answered (controls GPIO switching)
#define CALL_EVENT_HANGUP (1 << 2)   // Call ended (controls GPIO switching)
#define CALL_EVENT_DISPLAY (1 << 3)  // Display notification

// Call data structure - for passing call information
typedef struct
{
    char caller_id[32]; // Phone number or contact name
} CallData;

// Call message structure
typedef struct
{
    CallCommand cmd;
    void *data; // Points to CallData for commands that need it
} CallMessage;

// Opaque context
typedef struct CallStateContext CallStateContext;

// Public API
CallStateContext *CallState_Init(DisplayTaskContext *display_ctx);
void CallState_SetDisplayContext(CallStateContext *ctx, DisplayTaskContext *display_ctx);
bool CallState_PostCommand(CallStateContext *ctx, CallCommand cmd, void *data);
CallState CallState_GetCurrentState(CallStateContext *ctx);

#endif