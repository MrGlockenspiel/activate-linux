#ifndef INCLUDE_LOG_H
#define INCLUDE_LOG_H
#include <stdbool.h>

enum Verbosity {
    SILENT,
    ERROR,
    WARN,
    INFO,
    DEBUG
};


void inc_verbose();
void set_silent();
bool is_verbose(enum Verbosity level);

#define __debug__(FMTSTR, ...) if (is_verbose(DEBUG)) fprintf(stderr, "DEBUG: " FMTSTR, ## __VA_ARGS__)
#define __info__(FMTSTR, ...) if (is_verbose(INFO)) fprintf(stderr, "INFO: " FMTSTR, ## __VA_ARGS__)
#define __warn__(FMTSTR, ...) if (is_verbose(WARN)) fprintf(stderr, "WARN: " FMTSTR, ## __VA_ARGS__)
#define __error__(FMTSTR, ...) if (is_verbose(ERROR)) printf("ERROR: " FMTSTR, ## __VA_ARGS__)
#define __perror__(str) if (is_verbose(ERROR)) { __error__(str);perror("ERROR"); }

#endif
