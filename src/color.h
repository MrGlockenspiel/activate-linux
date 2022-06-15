#ifndef INCLUDE_COLOR_H
#define INCLUDE_COLOR_H

/**
 * The separator used in the CLI for color values.
 */
#define SEPARATOR "-"

/**
 * Struct representing an RGB color with alpha.
 *
 * All values range from 0 to 1, unless an error occurred, in which case the
 * error code will be stored in the alpha field as a negative number.
 */
struct rgba_color_t {
    float r, g, b, a;
};

/**
 * Constructs a new rgba_color_t with the given values.
 *
 * Once again, values range from 0 to 1. Invalid values will be clamped.
 *
 * @param r The red value.
 * @param g The green value.
 * @param b The blue value.
 * @param a The alpha value.
 *
 * @returns A new rgba_color_t with the given values.
 */
struct rgba_color_t rgba_color_new(float r, float g, float b, float a);

/**
 * Constructs a new rgba_color_t with the default values (r=1.0, g=1.0, b=1.0, a=0.35).
 *
 * @returns A new rgba_color_t with the default values.
 */
struct rgba_color_t rgba_color_default();

/**
 * Constructs a new rgba_color_t by parsing a string.
 * The string must be in the format "r-g-b-a", where r, g, b, and a are floats.
 *
 * The values may not be negative or greater than 1.
 *
 * @param str The string to parse.
 *
 * @returns A new rgba_color_t with the given values.
 */
struct rgba_color_t rgba_color_string(char *src);

#endif
