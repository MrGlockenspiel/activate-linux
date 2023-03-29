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
  int offset_left;
  int offset_top;

  rgba_color text_color;

  bool bypass_compositor;
  bool gamescope_overlay;
  bool daemonize;
  bool kill_running;
#ifdef X11
  bool force_xshape;
#endif
} Options;

extern Options options;

void parse_options(int argc, char *const argv[]);

#endif
