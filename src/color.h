#ifndef INCLUDE_COLOR_H
#define INCLUDE_COLOR_H

/**
 * The separator used in the CLI for color values.
 */
#define COLOR_SEPARATOR "-"

/**
 * Struct representing an RGB color with alpha.
 *
 * All values range from 0 to 1, unless an error occurred, in which case the
 * error code will be stored in the alpha field as a negative number.
 */
typedef struct rgba_color_t {
  float r, g, b, a;
} rgba_color;

/**
 * Constructs a new rgba_color with the given values.
 *
 * Once again, values range from 0 to 1. Invalid values will be clamped.
 *
 * @param r The red value.
 * @param g The green value.
 * @param b The blue value.
 * @param a The alpha value.
 *
 * @returns A new rgba_color with the given values.
 */
rgba_color rgba_color_new(float r, float g, float b, float a);

/**
 * Constructs a new rgba_color by parsing a string.
 * The string must be in the format "r-g-b-a", where r, g, b, and a are floats.
 *
 * The values may not be negative or greater than 1.
 *
 * @param str The string to parse.
 *
 * @returns A new rgba_color with the given values.
 */
rgba_color rgba_color_string(char *const src);

#endif
