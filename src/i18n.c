#include "i18n.h"

#include <stdlib.h>
#include <string.h>

#define i18n_map_size 4

char *i18n_map[i18n_map_size][4] = {
    { "zh_CN", "激活 ", "转到“设置”以激活 ", "。" },
    { "zh_TW", "啟用 ", "移至[設定]以啟用 ", "。" },
    { "zh_HK", "啟用 ", "移至[設定]以啟用 ", "。" },
};

char *another_strcat(size_t len, char *strings[])
{
    unsigned long t = 0;

    for (size_t i = 0; i < len; ++i) {
        t += strlen(strings[i]);
    }

    char *tmp = (char *) malloc((t + 1) * sizeof(char));
    tmp[0] = '\0';

    for (size_t i = 0; i < len; ++i) {
        strcat(tmp, strings[i]);
    }

    return tmp;
}

i18n_info i18n_get_info(char *system_name)
{
    char *title = "Activate ", *subtitle_pre = "Go to Settings to activate ",
         *subtitle_suf = ".";
    char *lang = getenv("LANG");

    for (size_t i = 0; i < i18n_map_size; ++i) {
        if (!strncmp(lang, i18n_map[i][0], 5)) {
            title = i18n_map[i][1];
            subtitle_pre = i18n_map[i][2], subtitle_suf = i18n_map[i][3];
            break;
        }
    }

    char *argv_title[] = { title, system_name };
    char *argv_subtitle[] = { subtitle_pre, system_name, subtitle_suf };

    // clang-format off
    return (i18n_info) {
        .title = another_strcat(2, argv_title),
        .subtitle = another_strcat(3, argv_subtitle),
    };
    // clang-format on
}
