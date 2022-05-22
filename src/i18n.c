#include "i18n.h"
#include <string.h>
#include <stdlib.h>

#define i18n_map_size 4
char *i18n_map[i18n_map_size][4] = {
    {"zh_CN", "激活 ", "转到“设置”以激活 ", "。"},
    {"zh_TW", "啟用 ", "移至[設定]以啟用 ", "。"},
    {"zh_HK", "啟用 ", "移至[設定]以啟用 ", "。"},
    {"ja_JP", " をアクティブ化", "「システム環境設定」 ", "をアクティブ化に行ってください。"},
};
char *another_strcat(size_t len, char *strings[]){
    unsigned long t = 0;
    for (size_t i = 0; i < len; ++i)
        t += strlen(strings[i]);
    char *tmp = (char*)malloc((t+1) * sizeof(char));
    tmp[0] = '\0';
    for (size_t i = 0; i < len; ++i)
        strcat(tmp, strings[i]);
    return tmp;
}
i18n_info i18n_get_info(char *system_name){
    char *title = "Activate ",
         *subtitle_pre = "Go to Settings to activate ",
         *subtitle_suf = ".";
    char *lang = getenv("LANG"); 
    for (size_t i = 0; i < i18n_map_size; ++i){
        if (!strncmp(lang, i18n_map[i][0], 5)){
            title = i18n_map[i][1];
            subtitle_pre = i18n_map[i][2],
            subtitle_suf = i18n_map[i][3];
            break;
        }
    }
    char *argv_title[] = {title, system_name};
    if (!strncmp(lang, "ja_JP", 5)) {
        argv_title[0] = system_name;
        argv_title[1] = title;
    }
    char *argv_subtitle[] = {subtitle_pre, system_name, subtitle_suf};
    return (i18n_info){another_strcat(2, argv_title), another_strcat(3, argv_subtitle)};
}