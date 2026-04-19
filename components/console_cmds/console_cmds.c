#include "console_cmds.h"

#include "nadeem_events.h"
#include "nadeem_config.h"
#include "backend_client.h"
#include "audio.h"
#include "semsem.h"
#include "stats.h"
#include "led.h"
#include "pn532.h"

#include "esp_console.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "argtable3/argtable3.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

static const char *TAG = "console";

/* ---- sys ------------------------------------------------------------- */

static int cmd_sys_info(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("nadeem firmware\n");
    printf("  uptime_ms  : %" PRIu64 "\n", (uint64_t)(esp_log_timestamp()));
    printf("  free_heap  : %" PRIu32 "\n", esp_get_free_heap_size());
    printf("  audio_state: %d  pos=%" PRIu32 "\n",
           (int)audio_current_state(), audio_current_position());
    printf("  led_mode   : %d\n", (int)led_get_mode());
    return 0;
}

static int cmd_sys_reboot(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("rebooting…\n");
    esp_restart();
    return 0;
}

/* ---- wifi ------------------------------------------------------------ */

static struct {
    struct arg_str *ssid;
    struct arg_str *pass;
    struct arg_end *end;
} s_wifi_set_args;

static int cmd_wifi_status(int argc, char **argv) {
    (void)argc; (void)argv;
    char ssid[64] = {0};
    if (nadeem_config_get_wifi(ssid, sizeof ssid, NULL, 0) == ESP_OK) {
        printf("ssid: %s\n", ssid);
    } else {
        printf("no ssid configured\n");
    }
    return 0;
}

static int cmd_wifi_set(int argc, char **argv) {
    int n = arg_parse(argc, argv, (void **)&s_wifi_set_args);
    if (n > 0) { arg_print_errors(stderr, s_wifi_set_args.end, "wifi set"); return 1; }
    esp_err_t err = nadeem_config_set_wifi(s_wifi_set_args.ssid->sval[0],
                                           s_wifi_set_args.pass->sval[0]);
    printf(err == ESP_OK ? "ok (reboot to apply)\n" : "failed\n");
    return err == ESP_OK ? 0 : 1;
}

static int cmd_wifi_forget(int argc, char **argv) {
    (void)argc; (void)argv;
    nadeem_config_clear();
    printf("cleared; reboot to re-onboard\n");
    return 0;
}

/* ---- nfc ------------------------------------------------------------- */

static int cmd_nfc_poll(int argc, char **argv) {
    (void)argc; (void)argv;
    pn532_uid_t uid;
    esp_err_t err = pn532_read_passive_target(&uid, 500);
    if (err == ESP_OK && uid.uid_len > 0) {
        printf("uid: ");
        for (int i = 0; i < uid.uid_len; i++) printf("%02x", uid.uid[i]);
        printf("\n");
    } else {
        printf("no target\n");
    }
    return 0;
}

/* ---- audio ----------------------------------------------------------- */

static struct {
    struct arg_str *path;
    struct arg_end *end;
} s_audio_play_args;

static int cmd_audio_play(int argc, char **argv) {
    int n = arg_parse(argc, argv, (void **)&s_audio_play_args);
    if (n > 0) { arg_print_errors(stderr, s_audio_play_args.end, "audio play"); return 1; }
    nev_audio_play_t p = {0};
    strncpy(p.path, s_audio_play_args.path->sval[0], sizeof p.path - 1);
    nadeem_events_post(NEV_AUDIO_PLAY, &p, sizeof p, 0);
    return 0;
}

static int cmd_audio_stop(int argc, char **argv)   { (void)argc; (void)argv; nadeem_events_post(NEV_AUDIO_STOP, NULL, 0, 0); return 0; }
static int cmd_audio_pause(int argc, char **argv)  { (void)argc; (void)argv; nadeem_events_post(NEV_AUDIO_PAUSE, NULL, 0, 0); return 0; }
static int cmd_audio_resume(int argc, char **argv) { (void)argc; (void)argv; nadeem_events_post(NEV_AUDIO_RESUME, NULL, 0, 0); return 0; }

static int cmd_audio_status(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("state=%d pos=%" PRIu32 " vol=%u\n",
           (int)audio_current_state(), audio_current_position(), audio_get_volume());
    return 0;
}

/* ---- semsem ---------------------------------------------------------- */

static bool stats_print_one(const char *uid, const stats_entry_t *e, void *user) {
    (void)user;
    printf("  %s  plays=%" PRIu32 "  total_ms=%" PRIu64 "  pro=%" PRIu32 "  dirty=%d\n",
           uid, e->play_count, e->total_play_ms, e->pro_session_count, (int)e->dirty);
    return true;
}

static int cmd_stats_dump(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("stats:\n");
    stats_for_each(stats_print_one, NULL);
    return 0;
}

static int cmd_stats_upload(int argc, char **argv) {
    (void)argc; (void)argv;
    esp_err_t err = backend_client_upload_stats();
    printf("upload: %s\n", esp_err_to_name(err));
    return err == ESP_OK ? 0 : 1;
}

static struct {
    struct arg_str *uid;
    struct arg_end *end;
} s_semsem_args;

static int cmd_semsem_trigger(int argc, char **argv) {
    int n = arg_parse(argc, argv, (void **)&s_semsem_args);
    if (n > 0) { arg_print_errors(stderr, s_semsem_args.end, "semsem trigger"); return 1; }
    esp_err_t err = semsem_trigger_uid(s_semsem_args.uid->sval[0]);
    printf("trigger: %s\n", esp_err_to_name(err));
    return err == ESP_OK ? 0 : 1;
}

/* ---- led ------------------------------------------------------------- */

static struct {
    struct arg_int *mode;
    struct arg_end *end;
} s_led_args;

static int cmd_led_mode(int argc, char **argv) {
    int n = arg_parse(argc, argv, (void **)&s_led_args);
    if (n > 0) { arg_print_errors(stderr, s_led_args.end, "led mode"); return 1; }
    esp_err_t err = led_set_mode((led_mode_t)s_led_args.mode->ival[0]);
    printf("%s\n", esp_err_to_name(err));
    return err == ESP_OK ? 0 : 1;
}

/* ---- registration ---------------------------------------------------- */

static void register_all(void) {
    const esp_console_cmd_t simple[] = {
        { .command = "sys_info",     .help = "print system info",    .func = cmd_sys_info },
        { .command = "sys_reboot",   .help = "reboot",               .func = cmd_sys_reboot },
        { .command = "wifi_status",  .help = "show Wi-Fi config",    .func = cmd_wifi_status },
        { .command = "wifi_forget",  .help = "clear Wi-Fi config",   .func = cmd_wifi_forget },
        { .command = "nfc_poll",     .help = "poll NFC once",        .func = cmd_nfc_poll },
        { .command = "audio_stop",   .help = "stop playback",        .func = cmd_audio_stop },
        { .command = "audio_pause",  .help = "pause playback",       .func = cmd_audio_pause },
        { .command = "audio_resume", .help = "resume playback",      .func = cmd_audio_resume },
        { .command = "audio_status", .help = "audio status",         .func = cmd_audio_status },
        { .command = "stats_dump",   .help = "dump usage stats",     .func = cmd_stats_dump },
        { .command = "stats_upload", .help = "force stats upload",   .func = cmd_stats_upload },
    };
    for (size_t i = 0; i < sizeof(simple)/sizeof(simple[0]); i++) {
        esp_console_cmd_register(&simple[i]);
    }

    s_wifi_set_args.ssid = arg_str1(NULL, NULL, "<ssid>", "SSID");
    s_wifi_set_args.pass = arg_str1(NULL, NULL, "<pass>", "password");
    s_wifi_set_args.end  = arg_end(2);
    esp_console_cmd_t wifi_set = {
        .command = "wifi_set", .help = "set Wi-Fi credentials",
        .func = cmd_wifi_set, .argtable = &s_wifi_set_args
    };
    esp_console_cmd_register(&wifi_set);

    s_audio_play_args.path = arg_str1(NULL, NULL, "<path>", "audio file path");
    s_audio_play_args.end  = arg_end(2);
    esp_console_cmd_t audio_play = {
        .command = "audio_play", .help = "play an audio file",
        .func = cmd_audio_play, .argtable = &s_audio_play_args
    };
    esp_console_cmd_register(&audio_play);

    s_semsem_args.uid = arg_str1(NULL, NULL, "<uid_hex>", "semsem UID hex");
    s_semsem_args.end = arg_end(2);
    esp_console_cmd_t semsem_trig = {
        .command = "semsem_trigger", .help = "simulate a semsem placement",
        .func = cmd_semsem_trigger, .argtable = &s_semsem_args
    };
    esp_console_cmd_register(&semsem_trig);

    s_led_args.mode = arg_int1(NULL, NULL, "<n>", "mode index");
    s_led_args.end  = arg_end(2);
    esp_console_cmd_t led_mode = {
        .command = "led_mode", .help = "force an LED mode",
        .func = cmd_led_mode, .argtable = &s_led_args
    };
    esp_console_cmd_register(&led_mode);
}

esp_err_t console_cmds_start(void) {
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_cfg = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_cfg.prompt = "nadeem> ";
    repl_cfg.max_cmdline_length = 128;

    esp_console_dev_uart_config_t uart_cfg = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    esp_err_t err = esp_console_new_repl_uart(&uart_cfg, &repl_cfg, &repl);
    if (err != ESP_OK) { ESP_LOGE(TAG, "repl init failed: %s", esp_err_to_name(err)); return err; }

    esp_console_register_help_command();
    register_all();
    return esp_console_start_repl(repl);
}
