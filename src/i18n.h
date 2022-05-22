#ifndef I18N
#define I18N
typedef struct
{
    char *title, *subtitle;
} i18n_info;

i18n_info i18n_get_info(char *system_name);
#endif
