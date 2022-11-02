#include <stdio.h>
#include "log.h"

static enum Verbosity __verbose__ = ERROR;

void inc_verbose(void) {
  if (__verbose__ < DEBUG) __verbose__++;
  // Print only high levels
  if (is_verbose_level(WARN)) print_verbose_level();
}

void set_silent(void) {
  __verbose__ = SILENT;
}

bool is_verbose_level(enum Verbosity level) {
  return level <= __verbose__;
}

void print_verbose_level(void) {
  char *level_str = "UNKNOWN?!";
  switch (__verbose__) {
    case SILENT: level_str = "SILENT"; break;
    case ERROR:  level_str = "ERROR";  break;
    case WARN:   level_str = "WARN";   break;
    case INFO:   level_str = "INFO";   break;
    case DEBUG:  level_str = "DEBUG";  break;
  }
  printf("Current verbosity level: %s\n", level_str);
}
