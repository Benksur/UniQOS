/**
 * @file call_state.h
 * @brief Call state machine task
 * @ingroup call_state_task
 *
 * Manages call state transitions and coordinates between cellular modem,
 * display, and audio systems during call operations.
 */

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

/** @ingroup call_state_task
 *  @brief Stack size for call state task in bytes */
#define CALL_STATE_TASK_STACK_SIZE 1024

/** @ingroup call_state_task
 *  @brief Call state task priority */
#define CALL_STATE_TASK_PRIORITY osPriorityAboveNormal

/**
 * @brief Call state enumeration
 * @ingroup call_state_task
 */
typedef enum
{
    CALL_STATE_IDLE,     /**< No active call */
    CALL_STATE_RINGING,  /**< Incoming call ringing */
    CALL_STATE_DIALLING, /**< Outgoing call dialing */
    CALL_STATE_ACTIVE,   /**< Call is active */
    CALL_STATE_ENDING    /**< Call is ending */
} CallState;

/**
 * @brief Call command enumeration
 * @ingroup call_state_task
 */
typedef enum
{
    CALL_CMD_INCOMING_CALL, /**< Handle incoming call */
    CALL_CMD_ANSWER_CALL,   /**< Answer incoming call */
    CALL_CMD_DIALLING,      /**< Start dialing */
    CALL_CMD_HANGUP_CALL,   /**< Hang up call */
} CallCommand;

/** @ingroup call_state_task
 *  @brief Event group bits for coordination */
#define CALL_EVENT_INCOMING (1 << 0) /**< Incoming call detected (controls vibration) */
#define CALL_EVENT_ANSWER (1 << 1)   /**< Call answered (controls GPIO switching) */
#define CALL_EVENT_HANGUP (1 << 2)   /**< Call ended (controls GPIO switching) */
#define CALL_EVENT_DISPLAY (1 << 3)  /**< Display notification */

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