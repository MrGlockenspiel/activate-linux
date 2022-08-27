#include "i18n.h"
#include "draw.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#define DEFAULT_PRESET 0
#elif __FreeBSD__
#define DEFAULT_PRESET 1
#else
#define DEFAULT_PRESET 2
#endif

#define match_str(L, P) strncmp(L, P, 5) == 0

#define platform_preset(L,TP,TS,SP,SS) \
    {L, "mac\0\0", TP "macOS" TS, SP "macOS" SS }, \
    {L, "bsd\0\0", TP "BSD"   TS, SP "BSD"   SS }, \
    {L, "linux"  , TP "Linux" TS, SP "Linux" SS }

static preset_map presets[] = {
    {"mac",   "Platform preset for macOS"}
    , {"bsd",   "Platform preset for *BSD"}
    , {"linux", "Platform preset for linux"}
};

static char *langs[] = {
    "en_US", "zh_CN", "zh_TW"
    , "zh_HK", "ru_RU", "ja_JP"
};
static size_t ilang = -1;

i18n_info i18n_map[] = {
    // Must be first, used as default
    platform_preset("en_US", "Activate ",, "Go to Settings to activate ", ".")
    , platform_preset("zh_CN", "激活 ",, "转到“设置”以激活 ", "。")
    , platform_preset("zh_TW", "啟用 ",, "移至[設定]以啟用 ", "。")
    , platform_preset("zh_HK", "啟用 ",, "移至[設定]以啟用 ", "。")
    , platform_preset("ru_RU", "Активация ",, "Чтобы активировать ", ",\nперейдите в раздел \"Параметры\".")
    , platform_preset("ja_JP",,"のライセンス認証", "設定を開き、", "のライセンス認証を行ってください")
};

void i18n_set_info(char* preset, struct draw_options* options)
{
    size_t preset_id = -1;

    if (ilang == (size_t)-1) {
        ilang = sizeof(langs) / sizeof(char*);
        char *lang = getenv("LANG");

        while(--ilang && ! match_str(langs[ilang], lang));
    }

    if (preset != NULL) {
        preset_id = sizeof(presets) / sizeof(preset_map);
        while(--preset_id && ! match_str(presets[preset_id].name, preset));
    }

    if(preset_id == (size_t)-1 || !match_str(presets[preset_id].name, preset))
        preset_id = DEFAULT_PRESET;

    if(preset_id + ilang * 3 < sizeof(i18n_map) / sizeof(i18n_info))
        preset_id += ilang * 3;

    options->title = i18n_map[preset_id].title;
    options->subtitle = i18n_map[preset_id].subtitle;
}

#define HELP(fmtstr, ...) fprintf(stderr, "  " fmtstr "\n", ## __VA_ARGS__)
#define STYLE(x) "\033[" # x "m"
#define END() fprintf(stderr, "\n")
void i18n_list_presets()
{
    size_t len = sizeof(presets) / sizeof(preset_map);
    fprintf(stderr, "Built-in Presets:\n\n");

    HELP(STYLE(1) "Name\t\tDescription" STYLE(0));
    while(len --) HELP(STYLE(1) "%s"STYLE(0) "\t\t%s", presets[len].name, presets[len].info);
}
#undef END
#undef STYLE
#undef HELP
