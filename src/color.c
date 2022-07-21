#include "color.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rgba_color_t rgba_color_new(float r, float g, float b, float a)
{
    struct rgba_color_t color = {
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    };

    return color;
}

struct rgba_color_t rgba_color_default()
{
    const struct rgba_color_t color = rgba_color_new(1.0, 1.0, 1.0, 0.35);

    return color;
}

struct rgba_color_t rgba_color_string(char *src)
{
    // Implementation note:
    //
    // Due to the way that the parsing is done, it automatically takes care of
    // the edge case of a negative value.

    struct rgba_color_t color = rgba_color_default();

    float *ptrs[4] = {
        &color.r,
        &color.g,
        &color.b,
        &color.a,
    };

    char *token;
    for (int i = 0; i < 4; i++) {
        // The compiler will (probably) optimize away this logic.
        if (i == 0) {
            token = strtok(src, SEPARATOR);
        } else {
            token = strtok(NULL, SEPARATOR);
        }

        if (token == NULL) {
            return rgba_color_new(0, 0, 0, -1.0);
        }

        float f = atof(token);
        // Clamp to 1.0 max.
        if (f > 1.0) {
            f = 1.0;
        }
        *ptrs[i] = f;
    }

    return color;
}
