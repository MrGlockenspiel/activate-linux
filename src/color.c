#include "color.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

rgba_color rgba_color_new(float r, float g, float b, float a) {
  __debug__("Creating color from: %f %f %f %f\n", r, g, b, a);
  rgba_color color = {
    .r = r,
    .g = g,
    .b = b,
    .a = a,
  };

  return color;
}

rgba_color rgba_color_string(char *const src) {
  __debug__("Creating color from string: %s\n", src);
  // Implementation note:
  //
  // Due to the way that the parsing is done, it automatically takes care of
  // the edge case of a negative value.

  rgba_color color;

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
      token = strtok(src, COLOR_SEPARATOR);
    } else {
      token = strtok(NULL, COLOR_SEPARATOR);
    }

    if (token == NULL) {
      return rgba_color_new(0, 0, 0, -1.0);
    }

    float f = atof(token);
    // Clamp to 1.0 max.
    if (f > 1.0) {
      f = 1.0;
    }

    __debug__("Got color component: %f\n", f);
    *ptrs[i] = f;
  }

  return color;
}
