#ifndef INCLUDE_OPTIONS_H
#define INCLUDE_OPTIONS_H

#include <stdbool.h>
#include <string.h>
#include "color.h"

typedef struct options_t {
  char *title;
  char *subtitle;
  char *custom_font;

  bool bold_mode;
  bool italic_mode;

  float scale;

  int overlay_width;
  int overlay_height;
  int overlay_offset_left;
  int overlay_offset_top;

  rgba_color text_color;

  bool bypass_compositor;
  bool gamescope_overlay;
  bool daemonize;
  bool kill_running;
#ifdef X11
  bool force_xshape;
  bool x11_draggable;
#endif
#ifdef WAYLAND
  bool wayland_draggable;
#endif
#ifdef LIBCONFIG
  char *config_file;
#endif
} Options;

extern Options options;

void parse_options(int argc, char *const argv[]);

#endif
