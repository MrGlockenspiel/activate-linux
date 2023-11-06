#include <libconfig.h>
#include "log.h"
#include "options.h"
#include "stdlib.h"
#include "i18n.h"

void load_config(const char *const file) {
  __debug__("Loading config from \"%s\"\n", file);
  config_t cfg, *cf;
  const char *tmp;
  int itmp;
  double ftmp;
  double ftmpa;
  double ftmpb;
  double ftmpc;

  cf = &cfg;
  config_init(cf);

  if (config_read_file(cf, file) == CONFIG_FALSE) {
    __error__("Config load failed. %s:%d - %s\n", config_error_file(cf), config_error_line(cf), config_error_text(cf));
    config_destroy(cf);
    exit(EXIT_FAILURE);
  }

  __debug__("Config read successfully\n");

  if (config_lookup_string(cf, "text-title", &tmp) != CONFIG_FALSE) {
    options.title = malloc(strlen(tmp) + 1);
    strcpy(options.title, tmp);
  }

  if (config_lookup_string(cf, "text-message", &tmp) != CONFIG_FALSE) {
    options.subtitle = malloc(strlen(tmp) + 1);
    strcpy(options.subtitle, tmp);
  }

  if (config_lookup_string(cf, "text-font", &tmp) != CONFIG_FALSE) {
    options.custom_font = malloc(strlen(tmp) + 1);
    strcpy(options.custom_font, tmp);
  }

  if (config_lookup_bool(cf, "text-bold", &itmp) != CONFIG_FALSE) {
    options.bold_mode = (bool)itmp;
  }

  if (config_lookup_bool(cf, "text-italic", &itmp) != CONFIG_FALSE) {
    options.italic_mode = (bool)itmp;
  }

  if (config_lookup_bool(cf, "bypass-compositor", &itmp) != CONFIG_FALSE) {
    options.bypass_compositor = (bool)itmp;
  }

  if (config_lookup_bool(cf, "gamescope", &itmp) != CONFIG_FALSE) {
    options.gamescope_overlay = (bool)itmp;
  }

  if (config_lookup_float(cf, "text-color-r", &ftmp) == CONFIG_FALSE) {
    ftmp = options.text_color.r;
  }

  if (config_lookup_float(cf, "text-color-g", &ftmpa) == CONFIG_FALSE) {
    ftmpa = options.text_color.g;
  }

  if (config_lookup_float(cf, "text-color-b", &ftmpb) == CONFIG_FALSE) {
    ftmpb = options.text_color.b;
  }

  if (config_lookup_float(cf, "text-color-a", &ftmpc) == CONFIG_FALSE) {
    ftmpc = options.text_color.a;
  }

  options.text_color = rgba_color_new((float)ftmp, (float)ftmpa, (float)ftmpb, (float)ftmpc);

  if (config_lookup_float(cf, "scale", &ftmp) != CONFIG_FALSE) {
    options.scale = ftmp;
  }

  if (config_lookup_int(cf, "overlay-width", &itmp) != CONFIG_FALSE) {
    options.overlay_width = itmp;
  }

  if (config_lookup_int(cf, "overlay-height", &itmp) != CONFIG_FALSE) {
    options.overlay_height = itmp;
  }

  if (config_lookup_int(cf, "overlay-offset-top", &itmp) != CONFIG_FALSE) {
    options.offset_top = itmp;
  }

  if (config_lookup_int(cf, "overlay-offset-left", &itmp) != CONFIG_FALSE) {
    options.offset_left = itmp;
  }

  if (config_lookup_bool(cf, "daemonize", &itmp) != CONFIG_FALSE) {
    options.daemonize = (bool)itmp;
  }
#ifdef X11
  if (config_lookup_bool(cf, "force-xshape", &itmp) != CONFIG_FALSE) {
    options.force_xshape = (bool)itmp;
  }
#endif
  if (config_lookup_bool(cf, "verbose", &itmp) != CONFIG_FALSE) {
    if (itmp) {
      inc_verbose();
    }
  }

  if (config_lookup_bool(cf, "quiet", &itmp) != CONFIG_FALSE) {
    if (itmp) {
      set_silent();
    }
  }

  if (config_lookup_string(cf, "text-preset", &tmp) != CONFIG_FALSE) {
    i18n_set_info(tmp);
  }

  config_destroy(cf);
}
