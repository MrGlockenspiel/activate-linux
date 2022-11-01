#ifndef I18N_H
#define I18N_H

typedef struct {
  char *title, *subtitle;
} i18n_info;

void i18n_set_info(const char *const preset);
void i18n_list_presets(void);

#endif
