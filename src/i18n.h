#ifndef I18N
#define I18N
#include "draw.h"

typedef struct {
    char *title, *subtitle;
} i18n_info;

void i18n_set_info(char* preset, struct draw_options* options);
void i18n_list_presets();
#endif
