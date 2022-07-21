#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "color.h"
#include "i18n.h"

#include "draw.h"
#include "log.h"
#include "x11.h"
#include "wayland.h"

int main(int argc, char *argv[])
{
    // title, subtitle text;
    i18n_info i18n = i18n_get_info();

    struct draw_options options = {
        .title = i18n.title,
        .subtitle = i18n.subtitle,
        .custom_font = "",
        .bold_mode = false,
        .slant_mode = false,

        .scale = 1.0f,

        // where the overlay appears
        .overlay_width = 340,
        .overlay_height = 120,
        .offset_left = 0,
        .offset_top = 0,

        // color of text - set default as light grey
        .text_color = rgba_color_default(),

        // bypass compositor hint
        .bypass_compositor = false,
    };

    // don't fork to background (default)
    bool daemonize = false;

    int opt;
    while ((opt = getopt(argc, argv, "h?vqbwdit:m:f:s:c:H:V:x:y:")) != -1) {
        switch (opt) {
        case 'v':
            inc_verbose();
            break;
        case 'q':
            set_silent();
            break;
        case 'b':
            options.bold_mode = true;
            break;
        case 'w':
            options.bypass_compositor = true;
            break;
        case 'd':
            daemonize = true;
            break;
        case 'i':
            options.slant_mode = true;
            break;
        case 't':
            options.title = optarg;
            break;
        case 'm':
            options.subtitle = optarg;
            break;
        case 'f':
            options.custom_font = optarg;
            break;
        case 's':
            options.scale = atof(optarg);
            if(options.scale < 0.0f) {
                fprintf(stderr, "Error occurred during parsing custom scale.\n");
                return 1;
            }
            break;
        case 'c':
            options.text_color = rgba_color_string(optarg);
            if (options.text_color.a < 0.0) {
                fprintf(stderr, "Error occurred during parsing custom color.\n");
                return 1;
            }
            break;
        case 'H':
            options.offset_left = atoi(optarg);
            break;
        case 'V':
            options.offset_top = atoi(optarg);
            break;
        case 'x':
            options.overlay_width = atoi(optarg);
            break;
        case 'y':
            options.overlay_height = atoi(optarg);
            break;
        case '?':
        case 'h':
#define HELP(fmtstr, ...) fprintf(stderr, "  " fmtstr "\n", ## __VA_ARGS__)
#define SECTION(name, fmtstr, ...) fprintf(stderr,(STYLE(1) "" name ": " STYLE(0) fmtstr "\n"), ## __VA_ARGS__)
#define END() fprintf(stderr, "\n")
#define STYLE(x) "\033[" # x "m"
#define COLOR(x, y) "\033[" # x ";" # y "m"
            SECTION("Usage", "%s [-biwdvq] [-c color] [-f font] [-m message] [-s scale] [-t title] ...", argv[0]);
            END();

            SECTION("Text", "");
            HELP("-t title\tSet  title  text (string)");
            HELP("-m message\tSet message text (string)");
            END();

            SECTION("Appearance", "");
            HELP("-f font\tSet the text font (string)");
            HELP("-b\t\tShow " STYLE(1) "bold" STYLE(0) " text");
            HELP("-i\t\tShow " STYLE(3) "italic/slanted" STYLE(0) " text");
            HELP("-c color\tSpecify color in " COLOR(1, 31) "r" STYLE(0)
                 "-" COLOR(1, 32) "g" STYLE(0) "-" COLOR(1,34) "b" STYLE(0)
                 "-" COLOR(1, 33) "a" STYLE(0)  " notation");
            HELP("\t\twhere " COLOR(1, 31) "r" STYLE(0) "/" COLOR(1,32)
                 "g" STYLE(0)  "/" COLOR(1, 34) "b" STYLE(0) "/" COLOR(1, 33)
                 "a" STYLE(0) " is between " COLOR(1, 32) "0.0" STYLE(0)
                 "-" COLOR(1, 34) "1.0" STYLE(0));
            END();

            SECTION("Geometry", "");
            HELP("-x width\tSet overlay width  before scaling (integer)");
            HELP("-y height\tSet overlay height before scaling (integer)");
            HELP("-s scale\tScale ratio (float)");
            HELP("-H offset\tMove overlay horizontally (integer)");
            HELP("-V offset\tMove overlay  vertically  (integer)");
            END();

            SECTION("Other", "");
            HELP("-w\t\tSet EWMH bypass_compositor hint");
            HELP("-d\t\tFork to background on startup");
            HELP("-v\t\tBe verbose and spam console");
            HELP("-q\t\tBe completely silent");
            END();
#undef HELP
#undef SECTION
#undef END
#undef STYLE
#undef COLOR
            exit(EXIT_SUCCESS);
        }
    }

    __debug__("Verbose mode activated\n");

    if (daemonize) {
        __debug__("Forking to background\n");
        int pid = -1;
        pid = fork();
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        } else if(pid == 0) {
            setsid();
        }
    }

    int wayland = wayland_backend_start(&options);
    // if the wayland backend fails, fallback to x11
    if (wayland == 0) {
        return 0;
    }

    return x11_backend_start(&options);
}
