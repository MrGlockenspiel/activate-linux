#include "log.h"

static enum Verbosity __verbose__ = ERROR;

void inc_verbose()
{
    if (__verbose__ <= DEBUG) __verbose__++;
}

void set_silent()
{
    if (__verbose__ > SILENT) __verbose__ = SILENT;
}

bool is_verbose(enum Verbosity level)
{
    return __verbose__ > level && level > SILENT;
}
