#include <libconfig.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
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
  if (config_lookup_int(cf, "overlay-offset-left", &itmp) != CONFIG_FALSE) {
    options.overlay_offset_left = itmp;
  }
  if (config_lookup_int(cf, "overlay-offset-top", &itmp) != CONFIG_FALSE) {
    options.overlay_offset_top = itmp;
  }

  if (config_lookup_bool(cf, "daemonize", &itmp) != CONFIG_FALSE) {
    options.daemonize = (bool)itmp;
  }
#ifdef X11
  if (config_lookup_bool(cf, "force-xshape", &itmp) != CONFIG_FALSE) {
    options.force_xshape = (bool)itmp;
  }
  if (config_lookup_bool(cf, "x11-draggable", &itmp) != CONFIG_FALSE) {
    options.x11_draggable = (bool)itmp;
  }
#endif
#ifdef WAYLAND
  if (config_lookup_bool(cf, "wayland-draggable", &itmp) != CONFIG_FALSE) {
    options.wayland_draggable = (bool)itmp;
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

int save_overlay_offsets(int offset_left, int offset_top) {
  config_t cfg, *cf;
  config_setting_t *root;
  config_setting_t *offset_left_setting;
  config_setting_t *offset_top_setting;
  bool config_exists;
  char default_cfg_path[PATH_MAX];

  if (options.config_file == NULL) {
    const char *home = getenv("HOME");
    char default_cfg_dir[PATH_MAX];
    if (home == NULL) {
      __error__("Cannot resolve HOME, failed to choose default config path\n");
      return 1;
    }

    snprintf(default_cfg_dir, sizeof(default_cfg_dir), "%s/.config", home);
    if (access(default_cfg_dir, F_OK) != 0 && mkdir(default_cfg_dir, 0700) != 0) {
      __error__("Failed to create config directory \"%s\"\n", default_cfg_dir);
      return 1;
    }

    snprintf(default_cfg_path, sizeof(default_cfg_path), "%s/.config/activate-linux.cfg", home);
    options.config_file = strdup(default_cfg_path);
    if (options.config_file == NULL) {
      __error__("Failed to allocate memory for default config path\n");
      return 1;
    }
  }

  config_exists = access(options.config_file, F_OK) == 0;

  cf = &cfg;
  config_init(cf);

  if (config_exists && config_read_file(cf, options.config_file) == CONFIG_FALSE) {
    __error__("Config load failed while saving offsets. %s:%d - %s\n",
              config_error_file(cf), config_error_line(cf), config_error_text(cf));
    config_destroy(cf);
    return 1;
  }

  root = config_root_setting(cf);
  offset_left_setting = config_lookup(cf, "overlay-offset-left");
  if (offset_left_setting == NULL) {
    offset_left_setting = config_setting_add(root, "overlay-offset-left", CONFIG_TYPE_INT);
  }
  offset_top_setting = config_lookup(cf, "overlay-offset-top");
  if (offset_top_setting == NULL) {
    offset_top_setting = config_setting_add(root, "overlay-offset-top", CONFIG_TYPE_INT);
  }

  if (offset_left_setting == NULL || offset_top_setting == NULL) {
    __error__("Failed to create config settings for overlay offsets\n");
    config_destroy(cf);
    return 1;
  }

  config_setting_set_int(offset_left_setting, offset_left);
  config_setting_set_int(offset_top_setting, offset_top);

  if (config_write_file(cf, options.config_file) == CONFIG_FALSE) {
    __error__("Failed to write config file \"%s\"\n", options.config_file);
    config_destroy(cf);
    return 1;
  }

  __debug__("Saved overlay offsets to \"%s\": left=%d top=%d\n",
            options.config_file, offset_left, offset_top);
  config_destroy(cf);
  return 0;
}
