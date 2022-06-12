#ifndef INCLUDE_DRAW_H
#define INCLUDE_DRAW_H

#include <stdbool.h>
#include <string.h>
#include <cairo.h>
#include "color.h"

void draw_text(cairo_t *cr, char *title, char *subtitle, float scale, struct rgba_color_t text_color, char* custom_font, bool bold_mode, bool slant_mode);

#endif
