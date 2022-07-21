#include "i18n.h"
#include <string.h>
#include <stdlib.h>

#ifdef __APPLE__
#define SYSTEM_NAME "macOS"
#elif __FreeBSD__
#define SYSTEM_NAME "BSD"
#else
#define SYSTEM_NAME "Linux"
#endif

i18n_info i18n_map[] = {
    // Must be first, used as default
    {"en_US", "Activate "SYSTEM_NAME, "Go to Settings to activate " SYSTEM_NAME "." },
    {"zh_CN", "激活 "SYSTEM_NAME, "转到“设置”以激活 "SYSTEM_NAME"。" },
    {"zh_TW", "啟用 "SYSTEM_NAME, "移至[設定]以啟用 "SYSTEM_NAME"。" },
    {"zh_HK", "啟用 "SYSTEM_NAME, "移至[設定]以啟用 "SYSTEM_NAME"。" },
    {"ru_RU", "Активация "SYSTEM_NAME, "Чтобы активировать "SYSTEM_NAME",\nперейдите в раздел \"Параметры\"." },
    {"ja_JP", SYSTEM_NAME"のライセンス認証", "設定を開き、" SYSTEM_NAME "のライセンス認証を行ってください" },
    {NULL, NULL, NULL }
};

i18n_info i18n_get_info()
{
    char *lang = getenv("LANG");
    for (size_t i = 0; i18n_map[i].lang; ++i) {
        if (!strncmp(lang, i18n_map[i].lang, 5)) {
            return i18n_map[i];
        }
    }
    return i18n_map[0];
}
