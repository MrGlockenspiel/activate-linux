#ifndef INCLUDE_LOG_H
#define INCLUDE_LOG_H

#include <stdio.h>
#include <stdbool.h>

enum Verbosity {
  SILENT,
  ERROR,
  WARN,
  INFO,
  DEBUG
};


void inc_verbose(void);
void set_silent(void);
bool is_verbose_level(enum Verbosity level);
void print_verbose_level(void);

#define __debug__(FMTSTR, ...) if (is_verbose_level(DEBUG)) fprintf(stderr, "DEBUG: " FMTSTR, ## __VA_ARGS__)
#define __info__(FMTSTR, ...)  if (is_verbose_level(INFO))  fprintf(stderr, "INFO:  " FMTSTR, ## __VA_ARGS__)
#define __warn__(FMTSTR, ...)  if (is_verbose_level(WARN))  fprintf(stderr, "WARN:  " FMTSTR, ## __VA_ARGS__)
#define __error__(FMTSTR, ...) if (is_verbose_level(ERROR)) printf("ERROR: " FMTSTR, ## __VA_ARGS__)
#define __perror__(str) if (is_verbose_level(ERROR)) { __error__(str);perror("ERROR"); }

#endif
