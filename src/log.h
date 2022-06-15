#ifndef INCLUDE_LOG_H
#define INCLUDE_LOG_H

static bool verbose_mode = 0;
#define verbose_printf(...) if (verbose_mode) printf(__VA_ARGS__)

#endif
