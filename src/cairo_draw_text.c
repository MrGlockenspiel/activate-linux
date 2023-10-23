#ifdef CAIRO

#include "cairo_draw_text.h"
#include "options.h"
#include <cairo/cairo.h>
#include <stdlib.h>

void draw_text(cairo_t *const cr, int xshape_mask)
{
    // clear surface
    cairo_operator_t prev_operator = cairo_get_operator(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    if (xshape_mask == 0)
    {
        cairo_set_operator(cr, prev_operator);
    }

    // set text color
    if (xshape_mask == 0)
    {
        cairo_set_source_rgba(cr, options.text_color.r, options.text_color.g, options.text_color.b,
                              options.text_color.a);
    }
    else
    {
        cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    }
    
    // cheap hack for xshape
    if (xshape_mask == 2)
    {
        cairo_set_source_rgb(cr, options.text_color.r, options.text_color.g, options.text_color.b);
        cairo_paint(cr);
        return;
    }
    
    // no subpixel anti-aliasing because we are on transparent BG
    cairo_font_options_t *font_options = cairo_font_options_create();
    if (xshape_mask == 0)
    {
        cairo_font_options_set_antialias(font_options, CAIRO_ANTIALIAS_GRAY);
    }
    else
    {
        cairo_font_options_set_antialias(font_options, CAIRO_ANTIALIAS_NONE);
    }
    cairo_set_font_options(cr, font_options);

    // set font size, and scale up or down
    cairo_set_font_size(cr, 24 * options.scale);

    // font weight and slant settings
    cairo_font_weight_t font_weight = CAIRO_FONT_WEIGHT_NORMAL;
    if (options.bold_mode)
    {
        font_weight = CAIRO_FONT_WEIGHT_BOLD;
    }

    cairo_font_slant_t font_slant = CAIRO_FONT_SLANT_NORMAL;
    if (options.italic_mode)
    {
        font_slant = CAIRO_FONT_SLANT_ITALIC;
    }

    cairo_select_font_face(cr, options.custom_font, font_slant, font_weight);

    cairo_move_to(cr, 20, 30 * options.scale);
    cairo_show_text(cr, options.title);

    cairo_set_font_size(cr, 16 * options.scale);
    cairo_move_to(cr, 20, 55 * options.scale);

    // handle string with \n as cairo cannot do it out of the box
    char *subtitle = options.subtitle;
    char *new_line_ptr = strchr(subtitle, '\n');
    if (new_line_ptr)
    {
        size_t first_line_len = new_line_ptr - subtitle;
        char *first_line = calloc(1, first_line_len + 1);
        memcpy(first_line, subtitle, first_line_len);
        cairo_show_text(cr, first_line);
        free(first_line);

        cairo_move_to(cr, 20, 75 * options.scale);
        cairo_show_text(cr, new_line_ptr + 1);
    }
    else
    {
        cairo_show_text(cr, subtitle);
    }

    cairo_font_options_destroy(font_options);
}

#endif
