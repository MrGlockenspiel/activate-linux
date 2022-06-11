#ifndef INCLUDE_DRAW_H
#define INCLUDE_DRAW_H

#include <stdbool.h>
#include <string.h>
#include <cairo.h>
#include "color.h"

struct draw_options {
    char *title;
    char *subtitle;
    char *custom_font;

    bool bold_mode;
    bool slant_mode;

    float scale;

    int overlay_width;
    int overlay_height;
    int offset_left;
    int offset_top;

    struct rgba_color_t text_color;

    bool bypass_compositor;
};

void draw_text(cairo_t *cr, struct draw_options *options);

#endif
