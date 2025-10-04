/**
 * @file power_task.h
 * @brief Power management task
 * @ingroup power_task
 *
 * FreeRTOS task that monitors battery status, manages power states,
 * and provides power-related information to other system components.
 */

#ifndef POWER_TASK_H_
#define POWER_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "display_task.h"

/** @ingroup power_task
 *  @brief Stack size for power task in bytes */
#define POWER_TASK_STACK_SIZE 512

/** @ingroup power_task
 *  @brief Power task priority */
#define POWER_TASK_PRIORITY osPriorityLow

/**
 * @brief Power command enumeration
 * @ingroup power_task
 */
typedef enum
{
    POWER_CMD_INIT,            /**< Initialize power management */
    POWER_CMD_GET_SOC,         /**< Get state of charge */
    POWER_CMD_GET_VOLTAGE,     /**< Get battery voltage */
    POWER_CMD_GET_TEMPERATURE, /**< Get battery temperature */
    POWER_CMD_SHUTDOWN,        /**< Shutdown system */
    POWER_CMD_STATS,           /**< Get power statistics */
} PowerCommand;

/**
 * @brief Power message structure
 * @ingroup power_task
 */
typedef struct
{
    PowerCommand cmd; /**< Command type */
    void *data;       /**< Optional command data */
} PowerMessage;

/**
 * @brief Opaque power task context
 * @ingroup power_task
 */
typedef struct PowerTaskContext PowerTaskContext;

/**
 * @ingroup power_task
 * @brief Initialize the power task
 * @param display_ctx Display task context
 * @return Pointer to power task context
 */
PowerTaskContext *PowerTask_Init(DisplayTaskContext *display_ctx);

/**
 * @ingroup power_task
 * @brief Post a command to the power task
 * @param ctx Power task context
 * @param cmd Command to post
 * @param data Optional command data
 * @return true if command was posted successfully
 */
bool PowerTask_PostCommand(PowerTaskContext *ctx, PowerCommand cmd, void *data);

#endif // POWER_TASK_H_
