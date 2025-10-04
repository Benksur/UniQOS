/**
 * @file display_task.h
 * @brief Display task for UI rendering
 * @ingroup display_task
 *
 * FreeRTOS task that handles all display rendering, UI updates, and user input events.
 * This task manages the screen buffer and coordinates with other tasks to display
 * system status, incoming calls, messages, and application pages.
 */

#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "screen.h"
#include "rtc.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "pages/menu.h"
#include "status_bar.h"
#include "task.h"
#include "incoming_call.h"

/** @ingroup display_task
 *  @brief Stack size for display task in bytes */
#define DISPLAY_TASK_STACK_SIZE 2048

/** @ingroup display_task
 *  @brief Display task priority */
#define DISPLAY_TASK_PRIORITY osPriorityNormal

/**
 * @brief Display task commands
 * @ingroup display_task
 */
typedef enum
{
    DISPLAY_HANDLE_INPUT,
    DISPLAY_SET_PAGE,
    DISPLAY_CLEAR_SCREEN,
    DISPLAY_SET_BATTERY_STATUS,
    DISPLAY_SET_SIGNAL_STATUS,
    DISPLAY_SET_VOLUME,
    DISPLAY_INCOMING_CALL,
    DISPLAY_ACTIVE_CALL,
    DISPLAY_CALL_ENDED,
    DISPLAY_DIALLING,
    DISPLAY_SHOW_SMS,
    DISPLAY_SET_BATTERY_PAGE,
    DISPLAY_SYNC_RTC,
    DISPLAY_CMD_COUNT
} DisplayCommand;

/**
 * @brief Display message structure
 * @ingroup display_task
 */
typedef struct
{
    DisplayCommand cmd; /**< Command type */
    void *data;         /**< Optional command data */
} DisplayMessage;

/**
 * @brief Display status information
 * @ingroup display_task
 */
typedef struct
{
    uint8_t battery_percent; /**< Battery charge level (0-100%) */
    uint8_t signal_strength; /**< Signal strength (0-5 bars) */
    RTC_TimeTypeDef time;    /**< Current time */
    RTC_DateTypeDef date;    /**< Current date */
} DisplayStatus;

/**
 * @brief RTC synchronization data
 * @ingroup display_task
 */
typedef struct
{
    RTC_TimeTypeDef time; /**< Time to sync */
    RTC_DateTypeDef date; /**< Date to sync */
} RtcSyncData;

// Forward declarations
typedef struct CallStateContext CallStateContext;
typedef struct CellularTaskContext CellularTaskContext;
typedef struct PowerTaskContext PowerTaskContext;

/**
 * @brief Opaque display task context
 * @ingroup display_task
 */
typedef struct DisplayTaskContext DisplayTaskContext;

/**
 * @ingroup display_task
 * @brief Initialize the display task
 * @param call_ctx Call state context pointer
 * @param cellular_ctx Cellular task context pointer
 * @return Pointer to display task context
 */
DisplayTaskContext *DisplayTask_Init(CallStateContext *call_ctx, CellularTaskContext *cellular_ctx);

/**
 * @ingroup display_task
 * @brief Set cellular task context
 * @param ctx Display task context
 * @param cellular_ctx Cellular task context to associate
 */
void DisplayTask_SetCellularContext(DisplayTaskContext *ctx, CellularTaskContext *cellular_ctx);

/**
 * @ingroup display_task
 * @brief Set power task context
 * @param ctx Display task context
 * @param power_ctx Power task context to associate
 */
void DisplayTask_SetPowerContext(DisplayTaskContext *ctx, PowerTaskContext *power_ctx);

/**
 * @ingroup display_task
 * @brief Post a command to the display task
 * @param ctx Display task context
 * @param cmd Command to post
 * @param data Optional command data
 * @return true if command was posted successfully
 */
bool DisplayTask_PostCommand(DisplayTaskContext *ctx, DisplayCommand cmd, void *data);

#endif // DISPLAY_TASK_H_