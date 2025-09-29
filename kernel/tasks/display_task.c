#include "display_task.h"
#include "display.h"
#include "input.h"
#include "pages/menu.h"
#include "status_bar.h"
#include "theme.h"
#include "tile.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
#include <string.h>

struct DisplayTaskContext
{
    QueueHandle_t queue;
};

typedef void (*DisplayCmdHandler)(DisplayTaskContext *ctx, DisplayMessage *msg);

/* ===== HANDLERS ===== */
static void handle_input_event(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    input_event_t *event = (input_event_t *)msg->data;
    if (event)
    {
        // Handle special cases like the test file
        if (*event == INPUT_RIGHT)
        {
            screen_pop_page();
        }
        else
        {
            screen_handle_input(*event);
        }
    }
}

static void handle_set_page(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    Page *page = (Page *)msg->data;
    if (page)
    {
        screen_set_page(page);
    }
}

static void handle_clear_screen(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    display_fill(COLOUR_BLACK);
}

static void handle_set_volume(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    uint8_t *volume = (uint8_t *)msg->data;
    if (volume)
    {
        status_bar_show_volume(*volume);
    }
}

static DisplayCmdHandler display_cmd_table[] = {
    [DISPLAY_HANDLE_INPUT] = handle_input_event,
    [DISPLAY_SET_PAGE] = handle_set_page,
    [DISPLAY_CLEAR_SCREEN] = handle_clear_screen,
    [DISPLAY_SET_VOLUME] = handle_set_volume,
};

static void dispatch_display_command(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    if (msg->cmd < (sizeof(display_cmd_table) / sizeof(display_cmd_table[0])))
    {
        DisplayCmdHandler handler = display_cmd_table[msg->cmd];
        if (handler)
        {
            handler(ctx, msg);
        }
    }
}

static void display_task_main(void *pvParameters)
{
    DisplayTaskContext *ctx = (DisplayTaskContext *)pvParameters;
    DisplayMessage msg;

    // Initialize display subsystem inside the task context to avoid blocking kernel startup
    display_init();
    osDelay(100);
    display_fill(COLOUR_BLACK);
    theme_set_dark();
    draw_status_bar();
    status_bar_update_signal(5);
    status_bar_update_battery(50);
    screen_init(&menu_page);
    mark_all_tiles_dirty();
    screen_tick();

    // Task main loop - handles messages and ticks like the test file
    for (;;)
    {
        if (xQueueReceive(ctx->queue, &msg, pdMS_TO_TICKS(100)))
        {
            dispatch_display_command(ctx, &msg);
        }
        // Update status bar and screen like the test file
        status_bar_tick();
        screen_tick();
    }
}

DisplayTaskContext *DisplayTask_Init(void)
{
    static DisplayTaskContext display_ctx;
    memset(&display_ctx, 0, sizeof(display_ctx));

    // Create queue
    display_ctx.queue = xQueueCreate(5, sizeof(DisplayMessage));
    if (!display_ctx.queue)
    {
        return NULL; // Failed to create queue
    }

    // Create and start the task
    osThreadAttr_t task_attr = {
        .name = "DisplayTask",
        .stack_size = 1024, // Increased stack size for display operations
        .priority = osPriorityNormal};

    osThreadId_t thread_id = osThreadNew(display_task_main, &display_ctx, &task_attr);
    if (!thread_id)
    {
        vQueueDelete(display_ctx.queue);
        return NULL; // Failed to create task
    }

    return &display_ctx;
}

bool DisplayTask_PostCommand(DisplayTaskContext *ctx, DisplayCommand cmd, void *data)
{
    if (!ctx || !ctx->queue)
        return false;

    DisplayMessage msg = {
        .cmd = cmd,
        .data = data};
    return xQueueSend(ctx->queue, &msg, pdMS_TO_TICKS(10)) == pdTRUE;
}