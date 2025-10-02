#include "audio_task.h"

int16_t tick[] = {
    // sharp attack
    0, 8000, 16380, 24000, 32760, 32760, 32760, 32760, 32760, 32760,
    // sustain
    30000, 28000, 26000, 24000, 22000, 20000, 18000, 16000, 14000, 12000,
    // decay
    10000, 8000, 6000, 4000, 3000, 2500, 2000, 1500, 1000, 800,
    600, 400, 300, 200, 150, 100, 75, 50, 25, 10,
    5, 2, 1, 0, 0, 0, 0, 0, 0, 0};

int16_t bloop_base[] = {
    0, 8148, 15715, 22237, 27311, 30620, 31963, 31277, 28634, 24224,
    18349, 11401, 3832, -3903, -11477, -18425, -24268, -28649, -31276, -31921,
    -30524, -27186, -22158, -15799, -8537};

typedef struct
{
    uint8_t volume_speaker;
    uint8_t volume_headphones;
    uint8_t volume_mic_int;
    uint8_t volume_mic_ext;
    bool output_muted;
    bool mic_muted;
    AudioOutput selected_output;
    AudioMic selected_mic;
} AudioUserSettings;

struct AudioTaskContext
{
    AudioUserSettings settings; // HIDDEN
    QueueHandle_t queue;
    const IAudioDriver_t *codec; // HIDDEN
};

typedef void (*AudioCmdHandler)(AudioTaskContext *ctx, AudioMessage *msg);

/* ===== HANDLERS ===== */
static void handle_volume_up(AudioTaskContext *ctx, AudioMessage *msg)
{
    if (ctx->settings.selected_output == AUDIO_OUTPUT_SPK)
    {
        uint8_t v = ctx->settings.volume_speaker;
        ctx->settings.volume_speaker = (v > 95) ? 100 : (uint8_t)(v + 5);
        ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
    }
    else
    {
        uint8_t v = ctx->settings.volume_headphones;
        ctx->settings.volume_headphones = (v > 95) ? 100 : (uint8_t)(v + 5);
        ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
    }

    // Send updated volume back to input task for display sync
    // Note: This would require a callback mechanism, but for now we'll rely on
    // the input task to keep track correctly
}

static void handle_volume_down(AudioTaskContext *ctx, AudioMessage *msg)
{
    if (ctx->settings.selected_output == AUDIO_OUTPUT_SPK)
    {
        uint8_t v = ctx->settings.volume_speaker;
        ctx->settings.volume_speaker = (v < 5) ? 0 : (uint8_t)(v - 5);
        ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
    }
    else
    {
        uint8_t v = ctx->settings.volume_headphones;
        ctx->settings.volume_headphones = (v < 5) ? 0 : (uint8_t)(v - 5);
        ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
    }
}

static void handle_mute_output(AudioTaskContext *ctx, AudioMessage *msg)
{
    ctx->settings.output_muted = true;
    ctx->codec->speaker.mute(true);
    ctx->codec->headphones.mute(true);
}

static void handle_unmute_output(AudioTaskContext *ctx, AudioMessage *msg)
{
    ctx->settings.output_muted = false;
    ctx->codec->speaker.mute(false);
    ctx->codec->headphones.mute(false);
}

static void handle_mute_mic(AudioTaskContext *ctx, AudioMessage *msg)
{
    ctx->settings.mic_muted = true;
    ctx->codec->speaker.mic.mute(true);
    ctx->codec->headphones.mic.mute(true);
}

static void handle_unmute_mic(AudioTaskContext *ctx, AudioMessage *msg)
{
    ctx->settings.mic_muted = false;
    ctx->codec->speaker.mic.mute(false);
    ctx->codec->headphones.mic.mute(false);
}

static void handle_set_volume_spk(AudioTaskContext *ctx, AudioMessage *msg)
{
    uint8_t volume = *((uint8_t *)msg->data);
    if (volume > 100)
        volume = 100;
    ctx->settings.volume_speaker = volume;
    if (ctx->settings.selected_output == AUDIO_OUTPUT_SPK && !ctx->settings.output_muted)
    {
        ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
    }
}

static void handle_set_volume_hp(AudioTaskContext *ctx, AudioMessage *msg)
{
    uint8_t volume = *((uint8_t *)msg->data);
    if (volume > 100)
        volume = 100;
    ctx->settings.volume_headphones = volume;
    if (ctx->settings.selected_output == AUDIO_OUTPUT_HP && !ctx->settings.output_muted)
    {
        ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
    }
}

static void handle_set_volume_mic(AudioTaskContext *ctx, AudioMessage *msg)
{
    uint8_t volume = *((uint8_t *)msg->data);
    if (volume > 100)
        volume = 100;
    ctx->settings.volume_mic_int = volume;
    if (ctx->settings.selected_mic == AUDIO_MIC_INT && !ctx->settings.mic_muted)
    {
        ctx->codec->speaker.mic.set_volume(ctx->settings.volume_mic_int);
    }
}

static void handle_set_volume_hp_mic(AudioTaskContext *ctx, AudioMessage *msg)
{
    uint8_t volume = *((uint8_t *)msg->data);
    if (volume > 100)
        volume = 100;
    ctx->settings.volume_mic_ext = volume;
    if (ctx->settings.selected_mic == AUDIO_MIC_HP && !ctx->settings.mic_muted)
    {
        ctx->codec->headphones.mic.set_volume(ctx->settings.volume_mic_ext);
    }
}

static void handle_select_output(AudioTaskContext *ctx, AudioMessage *msg)
{
    AudioOutput output = *((AudioOutput *)msg->data);
    ctx->settings.selected_output = output;
    if (!ctx->settings.output_muted)
    {
        if (output == AUDIO_OUTPUT_SPK)
        {
            ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
            ctx->codec->headphones.mute(true);
            ctx->codec->speaker.mute(false);
        }
        else
        {
            ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
            ctx->codec->speaker.mute(true);
            ctx->codec->headphones.mute(false);
        }
    }
}

static void handle_select_mic(AudioTaskContext *ctx, AudioMessage *msg)
{
    AudioMic mic = *((AudioMic *)msg->data);
    ctx->settings.selected_mic = mic;
    if (!ctx->settings.mic_muted)
    {
        if (mic == AUDIO_MIC_INT)
        {
            ctx->codec->speaker.mic.set_volume(ctx->settings.volume_mic_int);
            ctx->codec->headphones.mic.mute(true);
            ctx->codec->speaker.mic.mute(false);
        }
        else
        {
            ctx->codec->headphones.mic.set_volume(ctx->settings.volume_mic_ext);
            ctx->codec->speaker.mic.mute(true);
            ctx->codec->headphones.mic.mute(false);
        }
    }
}

static void handle_play_tick(AudioTaskContext *ctx, AudioMessage *msg)
{
    HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t *)tick, 50, HAL_MAX_DELAY);
}

static void handle_play_bloop(AudioTaskContext *ctx, AudioMessage *msg)
{
    // Play the base pattern multiple times to create the full bloop sound
    for (int i = 0; i < BLOOP_REPEAT_COUNT; i++)
    {
        HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t *)bloop_base, BLOOP_BASE_SIZE, HAL_MAX_DELAY);
    }
}

static void handle_get_current_volume(AudioTaskContext *ctx, AudioMessage *msg)
{
    // Return the current volume based on selected output
    uint8_t *volume_ptr = (uint8_t *)msg->data;
    if (volume_ptr)
    {
        if (ctx->settings.selected_output == AUDIO_OUTPUT_SPK)
        {
            *volume_ptr = ctx->settings.volume_speaker;
        }
        else
        {
            *volume_ptr = ctx->settings.volume_headphones;
        }
    }
}

/* ===== DISPATCH TABLE ===== */
static AudioCmdHandler audio_cmd_table[] = {
    // using designated initializers
    [AUDIO_VOLUME_UP] = handle_volume_up,
    [AUDIO_VOLUME_DOWN] = handle_volume_down,
    [AUDIO_MUTE_OUTPUT] = handle_mute_output,
    [AUDIO_UNMUTE_OUTPUT] = handle_unmute_output,
    [AUDIO_MUTE_MIC] = handle_mute_mic,
    [AUDIO_UNMUTE_MIC] = handle_unmute_mic,
    [AUDIO_SET_VOLUME_SPK] = handle_set_volume_spk,
    [AUDIO_SET_VOLUME_HP] = handle_set_volume_hp,
    [AUDIO_SET_VOLUME_MIC] = handle_set_volume_mic,
    [AUDIO_SET_VOLUME_HP_MIC] = handle_set_volume_hp_mic,
    [AUDIO_SELECT_OUTPUT] = handle_select_output,
    [AUDIO_SELECT_MIC] = handle_select_mic,
    [AUDIO_PLAY_TICK] = handle_play_tick,
    [AUDIO_PLAY_BLOOP] = handle_play_bloop,
    [AUDIO_GET_CURRENT_VOLUME] = handle_get_current_volume,
};

static void dispatch_audio_command(AudioTaskContext *ctx, AudioMessage *msg)
{
    if (msg->cmd < (sizeof(audio_cmd_table) / sizeof(audio_cmd_table[0])))
    {
        AudioCmdHandler handler = audio_cmd_table[msg->cmd];
        if (handler)
        {
            handler(ctx, msg);
        }
    }
}

static void audio_task_main(void *pvParameters)
{
    AudioTaskContext *ctx = (AudioTaskContext *)pvParameters;
    AudioMessage msg;

    ctx->codec = nau88c22_get_driver();
    ctx->codec->init();
    ctx->settings.volume_speaker = 100;
    ctx->settings.volume_headphones = 50;
    ctx->settings.volume_mic_int = 50;
    ctx->settings.volume_mic_ext = 50;
    ctx->settings.output_muted = false;
    ctx->settings.mic_muted = false;
    ctx->settings.selected_output = AUDIO_OUTPUT_SPK;
    ctx->settings.selected_mic = AUDIO_MIC_INT;

    ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
    // ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
    // ctx->codec->speaker.mic.set_volume(ctx->settings.volume_mic_int);
    // ctx->codec->headphones.mic.set_volume(ctx->settings.volume_mic_ext);
    // ctx->codec->speaker.mute(ctx->settings.output_muted);
    // ctx->codec->headphones.mute(ctx->settings.output_muted);
    // ctx->codec->speaker.mic.mute(ctx->settings.mic_muted);
    // ctx->codec->headphones.mic.mute(ctx->settings.mic_muted);

    for (;;)
    {
        if (xQueueReceive(ctx->queue, &msg, portMAX_DELAY))
        {
            dispatch_audio_command(ctx, &msg);
        }
        osDelay(10);
    }
}

AudioTaskContext *AudioTask_Init(void)
{
    static AudioTaskContext audio_ctx;
    memset(&audio_ctx, 0, sizeof(audio_ctx));
    audio_ctx.queue = xQueueCreate(5, sizeof(AudioMessage));

    osThreadAttr_t task_attr = {
        .name = "AudioTask",
        .stack_size = AUDIO_TASK_STACK_SIZE,
        .priority = AUDIO_TASK_PRIORITY};
    osThreadNew(audio_task_main, &audio_ctx, &task_attr);

    return &audio_ctx;
}

bool AudioTask_PostCommand(AudioTaskContext *ctx, AudioCommand cmd, void *data)
{
    if (!ctx || !ctx->queue)
        return false;

    AudioMessage msg = {
        .cmd = cmd,
        .data = data};
    return xQueueSend(ctx->queue, &msg, 0) == pdTRUE;
}