#include "display_task.h"
#include "display.h"
#include "input.h"
#include "pages/menu.h"
#include <string.h>

struct DisplayTaskContext {
    QueueHandle_t queue;
};

typedef void (*DisplayCmdHandler)(DisplayTaskContext* ctx, DisplayMessage* msg);

/* ===== HANDLERS ===== */
static void handle_input_event(DisplayTaskContext* ctx, DisplayMessage* msg) {
    input_event_t* event = (input_event_t*)msg->data;
    if (event) {
        screen_handle_input(event);
    }
}

static void handle_set_page(DisplayTaskContext* ctx, DisplayMessage* msg) {
    Page* page = (Page*)msg->data;
    if (page) {
        screen_set_page(page);
    }
}

static void handle_clear_screen(DisplayTaskContext* ctx, DisplayMessage* msg) {
    display_fill(COLOUR_BLACK);
}

static DisplayCmdHandler display_cmd_table[] = {
    [DISPLAY_HANDLE_INPUT] = handle_input_event,
    [DISPLAY_SET_PAGE] = handle_set_page,
    [DISPLAY_CLEAR_SCREEN] = handle_clear_screen,
};

static void dispatch_display_command(DisplayTaskContext* ctx, DisplayMessage* msg) {
    if (msg->cmd < (sizeof(display_cmd_table) / sizeof(display_cmd_table[0]))) {
        DisplayCmdHandler handler = display_cmd_table[msg->cmd];
        if (handler) {
            handler(ctx, msg);
        }
    }
}

static void display_task_main(void *pvParameters) {
    DisplayTaskContext* ctx = (DisplayTaskContext*)pvParameters;
    DisplayMessage msg;

    // Task main loop - only handles messages and ticks
    for(;;) {
        if (xQueueReceive(ctx->queue, &msg, pdMS_TO_TICKS(100))) {
            dispatch_display_command(ctx, &msg);
        }
        screen_tick();
    }
}

DisplayTaskContext* DisplayTask_Init(void) {
    static DisplayTaskContext display_ctx;
    memset(&display_ctx, 0, sizeof(display_ctx));

    // Initialize hardware and state
    display_init();
    display_fill(COLOUR_BLACK);
    theme_set_dark();
    screen_init(&menu_page);

    // Create queue
    display_ctx.queue = xQueueCreate(5, sizeof(DisplayMessage));

    // Create and start the task
    osThreadAttr_t task_attr = {
        .name = "DisplayTask",
        .stack_size = 512,
        .priority = osPriorityNormal
    };
    osThreadNew(display_task_main, &display_ctx, &task_attr);

    return &display_ctx;
}

bool DisplayTask_PostCommand(DisplayTaskContext* ctx, DisplayCommand cmd, void* data) {
    if (!ctx || !ctx->queue) return false;

    DisplayMessage msg = {
        .cmd = cmd,
        .data = data
    };
    return xQueueSend(ctx->queue, &msg, pdMS_TO_TICKS(10)) == pdTRUE;
}