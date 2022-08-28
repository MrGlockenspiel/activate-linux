#include "i18n.h"
#include "i18n-helper.h"
#include "draw.h"
#include "log.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// FIXME: You must follow the order of presets[] in i18n_map[]
static preset_map presets[] = {
    map_preset("mac",   "Platform preset for macOS")
    map_preset("bsd",   "Platform preset for *BSD")
    map_preset("linux", "Platform preset for linux")
    map_preset("m$",    "Diss M!cr0$0f+")
};

// FIXME: You must follow the order of langs[] in i18n_map[][]
static lang_map langs[] = {
    _LANG("en", "US")
    _LANG("zh", "CN")
    _LANG("zh", "TW")
    _LANG("zh", "HK")
    _LANG("ru", "RU")
    _LANG("ja", "JP")
};

// selected language id
static size_t ilang = -1;

// FIXME: You must follow the order of langs[] here
#define platform_preset(platform) \
    {\
        platform_i18n(platform, "en_US", "Activate ",, "Go to Settings to activate ", ".") \
        platform_i18n(platform, "zh_CN", "激活 ",, "转到“设置”以激活 ", "。") \
        platform_i18n(platform, "zh_TW", "啟用 ",, "移至[設定]以啟用 ", "。") \
        platform_i18n(platform, "zh_HK", "啟用 ",, "移至[設定]以啟用 ", "。") \
        platform_i18n(platform, "ru_RU", "Активация ",, "Чтобы активировать ", ",\nперейдите в раздел \"Параметры\".") \
        platform_i18n(platform, "ja_JP",,"のライセンス認証", "設定を開き、", "のライセンス認証を行ってください") \
    },

// FIXME: You must follow the order of langs[] here
#define ms_diss_preset() \
    { \
        system_i18n ("en_US", "No need to activate ",, "We're not as annoying as Microsoft.") \
        untranslated("zh_CN") \
        untranslated("zh_TW") \
        untranslated("zh_HK") \
        untranslated("ru_RU") \
        untranslated("ja_JP") \
    },

// FIXME: You must follow the order of presets[] here
i18n_info i18n_map[][length(langs)] = {
    platform_preset("macOS") // macOS platform preset
    platform_preset("BSD")   // BSD platform preset
    platform_preset("Linux") // Linux platform preset
    ms_diss_preset()         // ms-diss system preset
};

void i18n_set_info(char* preset, struct draw_options* options)
{
    size_t ipreset = DEFAULT_PRESET;

    if (ilang == (size_t)-1) {
        ilang = length(langs);
        char *lang = getenv("LANG");
        while(--ilang && ! match_str(langs[ilang], lang));
    }

    if (preset != NULL) {
        ipreset = length(presets);
        while(--ipreset && ! match_str(presets[ipreset].name, preset));
        if(!match_str(presets[ipreset].name, preset)) {
            __warn__("Undefined preset: %s\n", preset);
            ipreset = DEFAULT_PRESET;
        }
    }

    __info__("Loading preset: %s\n", presets[ipreset].name);
    if (!(PRESET.title && PRESET.subtitle))
        ilang = 0;

    options->title = PRESET.title;
    options->subtitle = PRESET.subtitle;
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
