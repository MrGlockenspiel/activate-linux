#ifndef I18N_H
#define I18N_H

// language-specific structs
typedef struct {
  char *pre_title;
  char *post_title;
  char *pre_subtitle;
  char *post_subtitle;
} i18n_info_text;

typedef struct {
  char *pre_title;
  char *post_title;
  char *subtitle;
} i18n_info_diss_text;

typedef struct {
  const char *code;
  i18n_info_text windows_like;
  i18n_info_diss_text diss;
} i18n_info_soup;

// preset-specific structs
typedef struct {
  char *name;
  char *text;
} preset_t;


void i18n_set_info(const char *const preset);
void i18n_list_presets(void);

#endif
