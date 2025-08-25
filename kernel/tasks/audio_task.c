#include "audio_task.h"
#include "cmsis_os2.h"
#include <string.h>


typedef void (*AudioCmdHandler)(AudioTaskContext* ctx, AudioMessage* msg);

/* ===== HANDLERS ===== */
static void handle_volume_up(AudioTaskContext* ctx, AudioMessage* msg) {
    if (ctx->settings.selected_output == AUDIO_OUTPUT_SPK) {
        if (ctx->settings.volume_speaker < 100) ctx->settings.volume_speaker += 5;
        ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
    } else {
        if (ctx->settings.volume_headphones < 100) ctx->settings.volume_headphones += 5;
        ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
    }
}

static void handle_volume_down(AudioTaskContext* ctx, AudioMessage* msg) {
    if (ctx->settings.selected_output == AUDIO_OUTPUT_SPK) {
        if (ctx->settings.volume_speaker > 0) ctx->settings.volume_speaker -= 5;
        ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
    } else {
        if (ctx->settings.volume_headphones > 0) ctx->settings.volume_headphones -= 5;
        ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
    }
}

static void handle_mute_output(AudioTaskContext* ctx, AudioMessage* msg) {
    ctx->settings.output_muted = true;
    ctx->codec->speaker.mute(true);
    ctx->codec->headphones.mute(true);
}

static void handle_unmute_output(AudioTaskContext* ctx, AudioMessage* msg) {
    ctx->settings.output_muted = false;
    ctx->codec->speaker.mute(false);
    ctx->codec->headphones.mute(false);
}

static void handle_mute_mic(AudioTaskContext* ctx, AudioMessage* msg) {
    ctx->settings.mic_muted = true;
    ctx->codec->speaker.mic.mute(true);
    ctx->codec->headphones.mic.mute(true);
}

static void handle_unmute_mic(AudioTaskContext* ctx, AudioMessage* msg) {
    ctx->settings.mic_muted = false;
    ctx->codec->speaker.mic.mute(false);
    ctx->codec->headphones.mic.mute(false);
}

static void handle_set_volume_spk(AudioTaskContext* ctx, AudioMessage* msg) {
    uint8_t volume = *((uint8_t*)msg->data);
    if (volume > 100) volume = 100;
    ctx->settings.volume_speaker = volume;
    if (ctx->settings.selected_output == AUDIO_OUTPUT_SPK && !ctx->settings.output_muted) {
        ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
    }
}

static void handle_set_volume_hp(AudioTaskContext* ctx, AudioMessage* msg) {
    uint8_t volume = *((uint8_t*)msg->data);
    if (volume > 100) volume = 100;
    ctx->settings.volume_headphones = volume;
    if (ctx->settings.selected_output == AUDIO_OUTPUT_HP && !ctx->settings.output_muted) {
        ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
    }
}

static void handle_set_volume_mic(AudioTaskContext* ctx, AudioMessage* msg) {
    uint8_t volume = *((uint8_t*)msg->data);
    if (volume > 100) volume = 100;
    ctx->settings.volume_mic_int = volume;
    if (ctx->settings.selected_mic == AUDIO_MIC_INT && !ctx->settings.mic_muted) {
        ctx->codec->speaker.mic.set_volume(ctx->settings.volume_mic_int);
    }
}

static void handle_set_volume_hp_mic(AudioTaskContext* ctx, AudioMessage* msg) {
    uint8_t volume = *((uint8_t*)msg->data);
    if (volume > 100) volume = 100;
    ctx->settings.volume_mic_ext = volume;
    if (ctx->settings.selected_mic == AUDIO_MIC_HP && !ctx->settings.mic_muted) {
        ctx->codec->headphones.mic.set_volume(ctx->settings.volume_mic_ext);
    }
}

static void handle_select_output(AudioTaskContext* ctx, AudioMessage* msg) {
    AudioOutput output = *((AudioOutput*)msg->data);
    ctx->settings.selected_output = output;
    if (!ctx->settings.output_muted) {
        if (output == AUDIO_OUTPUT_SPK) {
            ctx->codec->speaker.set_volume(ctx->settings.volume_speaker);
            ctx->codec->headphones.mute(true);
            ctx->codec->speaker.mute(false);
        } else {
            ctx->codec->headphones.set_volume(ctx->settings.volume_headphones);
            ctx->codec->speaker.mute(true);
            ctx->codec->headphones.mute(false);
        }
    }
}

static void handle_select_mic(AudioTaskContext* ctx, AudioMessage* msg) {
    AudioMic mic = *((AudioMic*)msg->data);
    ctx->settings.selected_mic = mic;
    if (!ctx->settings.mic_muted) {
        if (mic == AUDIO_MIC_INT) {
            ctx->codec->speaker.mic.set_volume(ctx->settings.volume_mic_int);
            ctx->codec->headphones.mic.mute(true);
            ctx->codec->speaker.mic.mute(false);
        } else {
            ctx->codec->headphones.mic.set_volume(ctx->settings.volume_mic_ext);
            ctx->codec->speaker.mic.mute(true);
            ctx->codec->headphones.mic.mute(false);
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
};

static void dispatch_audio_command(AudioTaskContext* ctx, AudioMessage* msg) {
    if (msg->cmd < (sizeof(audio_cmd_table) / sizeof(audio_cmd_table[0]))) {
        AudioCmdHandler handler = audio_cmd_table[msg->cmd];
        if (handler) {
            handler(ctx, msg);
        }
    }
}

static void audio_task_main(void *pvParameters) {
    AudioTaskContext* ctx = (AudioTaskContext*)pvParameters;
    AudioMessage msg;

    for(;;) {
        if (xQueueReceive(ctx->queue, &msg, portMAX_DELAY)) {
            dispatch_audio_command(ctx, &msg);
        }
    }
}

AudioTaskContext* AudioTask_Init(void) {
    static AudioTaskContext audio_ctx;
    memset(&audio_ctx, 0, sizeof(audio_ctx));

    audio_ctx.settings.volume_speaker = 50;
    audio_ctx.settings.volume_headphones = 50;
    audio_ctx.settings.volume_mic_int = 50;
    audio_ctx.settings.volume_mic_ext = 50;
    audio_ctx.settings.output_muted = false;
    audio_ctx.settings.mic_muted = false;
    audio_ctx.settings.selected_output = AUDIO_OUTPUT_SPK;
    audio_ctx.settings.selected_mic = AUDIO_MIC_INT;

    audio_ctx.queue = xQueueCreate(8, sizeof(AudioMessage));
    audio_ctx.codec = (IAudioDriver_t*)nau88c22_get_driver();
    audio_ctx.codec->init();

    osThreadAttr_t task_attr = {
        .name = "AudioTask",
        .stack_size = 512,
        .priority = osPriorityNormal
    };
    osThreadNew(audio_task_main, &audio_ctx, &task_attr);

    return &audio_ctx;
}

bool AudioTask_PostCommand(AudioTaskContext* ctx, AudioCommand cmd, void* data) {
    if (!ctx || !ctx->queue) return false;

    AudioMessage msg = {
        .cmd = cmd,
        .data = data
    };
    return xQueueSend(ctx->queue, &msg, 0) == pdTRUE;
}