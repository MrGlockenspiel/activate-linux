#ifdef CAIRO

#include "cairo_draw_text.h"
#include "options.h"
#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <stdlib.h>

void draw_text(cairo_t *const cr, int xshape_mask)
{
    PangoLayout *layout = pango_cairo_create_layout(cr);
    PangoFontDescription *font_description = pango_font_description_new();
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

    // set font size, and scale up or down
    pango_font_description_set_absolute_size(font_description, 24 * PANGO_SCALE * options.scale);

    // font weight and slant settings
    PangoWeight font_weight = PANGO_WEIGHT_NORMAL;
    if (options.bold_mode)
    {
        font_weight = PANGO_WEIGHT_BOLD;
    }

    PangoStyle font_style = PANGO_STYLE_NORMAL;
    if (options.italic_mode)
    {
        font_style = PANGO_STYLE_ITALIC;
    }

    pango_font_description_set_family(font_description, options.custom_font);
    pango_font_description_set_weight(font_description, font_weight);
    pango_font_description_set_style(font_description, font_style);

    pango_layout_set_font_description(layout, font_description);
    pango_cairo_context_set_font_options(pango_layout_get_context(layout), font_options);

    cairo_move_to(cr, 20, 30 * options.scale);
    pango_layout_set_text(layout, options.title, -1);
    pango_cairo_show_layout_line(cr, pango_layout_get_line(layout, 0));

    pango_font_description_set_absolute_size(font_description, 16 * PANGO_SCALE * options.scale);
    pango_layout_set_font_description(layout, font_description);
    cairo_move_to(cr, 20, 55 * options.scale);

    // handle string with \n as cairo cannot do it out of the box
    char *subtitle = options.subtitle;
    char *new_line_ptr = strchr(subtitle, '\n');
    if (new_line_ptr)
    {
        size_t first_line_len = new_line_ptr - subtitle;
        char *first_line = calloc(1, first_line_len + 1);
        memcpy(first_line, subtitle, first_line_len);
        pango_layout_set_text(layout, first_line, -1);
        pango_cairo_show_layout_line(cr, pango_layout_get_line(layout, 0));
        free(first_line);

        cairo_move_to(cr, 20, 75 * options.scale);
        pango_layout_set_text(layout, new_line_ptr + 1, -1);
        pango_cairo_show_layout_line(cr, pango_layout_get_line(layout, 0));
    }
    else
    {
        pango_layout_set_text(layout, subtitle, -1);
        pango_cairo_show_layout_line(cr, pango_layout_get_line(layout, 0));
    }

    g_object_unref(layout);
    cairo_font_options_destroy(font_options);
    pango_font_description_free(font_description);
}

#endif
