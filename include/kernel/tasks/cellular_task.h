/**
 * @file cellular_task.h
 * @brief Cellular modem management task
 * @ingroup cellular_task
 *
 * FreeRTOS task that manages cellular modem operations including
 * call handling, SMS operations, and network status monitoring.
 */

#ifndef CELLULAR_TASK_H
#define CELLULAR_TASK_H

#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "display_task.h"
#include "call_state.h"
#include "modem.h"
#include "stm32_config.h"
#include "sms_types.h"

/** @ingroup cellular_task
 *  @brief Stack size for cellular task in bytes */
#define CELLULAR_TASK_STACK_SIZE 2048

/** @ingroup cellular_task
 *  @brief Cellular task priority */
#define CELLULAR_TASK_PRIORITY osPriorityNormal

/**
 * @brief Cellular command enumeration
 * @ingroup cellular_task
 */
typedef enum
{
    CELLULAR_CMD_INIT,              /**< Initialize modem */
    CELLULAR_CMD_SLEEP,             /**< Put modem to sleep */
    CELLULAR_CMD_AIRPLANE_MODE_ON,  /**< Enable airplane mode */
    CELLULAR_CMD_AIRPLANE_MODE_OFF, /**< Disable airplane mode */
    CELLULAR_CMD_SEND_SMS,          /**< Send SMS message */
    CELLULAR_CMD_DIAL,              /**< Dial phone number */
    CELLULAR_CMD_HANG_UP,           /**< Hang up call */
} CellularCommand;

/**
 * @brief Cellular message structure
 * @ingroup cellular_task
 */
typedef struct
{
    CellularCommand cmd; /**< Command type */
    void *data;          /**< Optional command data */
} CellularMessage;

/**
 * @brief Opaque cellular task context
 * @ingroup cellular_task
 */
typedef struct CellularTaskContext CellularTaskContext;

/**
 * @ingroup cellular_task
 * @brief Initialize the cellular task
 * @param display_ctx Display task context
 * @param call_ctx Call state context
 * @return Pointer to cellular task context
 */
CellularTaskContext *CellularTask_Init(DisplayTaskContext *display_ctx, CallStateContext *call_ctx);

/**
 * @ingroup cellular_task
 * @brief Post a command to the cellular task
 * @param ctx Cellular task context
 * @param cmd Command to post
 * @param data Optional command data
 * @return true if command was posted successfully
 */
bool CellularTask_PostCommand(CellularTaskContext *ctx, CellularCommand cmd, void *data);

#endif