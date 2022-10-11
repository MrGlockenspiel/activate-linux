#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>

#include "color.h"
#include "i18n.h"

#include "draw.h"
#include "log.h"
#include "x11/x11.h"
#include "wayland/wayland.h"

#if defined(NO_WAYLAND) && defined(NO_X11)
#error "Either Wayland or X11 backend must be enabled."
#endif

#if defined(NO_X11) || defined(NO_WAYLAND)
#define try_backend(X, ...) return  X ## _backend_start(__VA_ARGS__)
#else
#define try_backend(X, ...) \
    const int ret_ ## X =  X ## _backend_start(__VA_ARGS__); \
    if (ret_ ## X == 0 || strncmp(# X, "x11", 3) == 0) return ret_ ## X
#endif

int main(int argc, char *argv[])
{
	int option_index = 0;
    const struct option long_options[] = {
		{ "text-title", 1, NULL, 't' },
		{ "text-message", 1, NULL, 'm' },
		{ "text-preset", 1, NULL, 'p' },
		{ "text-font", 1, NULL, 'f' },
		{ "text-bold", 0, NULL, 'b' },
		{ "text-italic", 0, NULL, 'i' },
		{ "text-color", 1, NULL, 'c' },
	    { "overlay-width", 1, NULL, 'x' },
		{ "overlay-height", 1, NULL, 'y' },
	    { "overlay-offset-top", 1, NULL, 'V' },
		{ "overlay-offset-left", 1, NULL, 'H' },
		{ "scale", 1, NULL, 's' },
		{ "daemonize", 0, NULL, 'd' },
		{ "skip-compositior", 0, NULL, 'w' },
		{ "verbose", 0, NULL, 'v' },
		{ "text-preset-list", 0, NULL, 'l' },
		{ "quiet", 0, NULL, 'q' },
		{ "gamescope", 0, NULL, 'G' },
		{ "help", 0, NULL, 'h' },
		{ NULL, 0, NULL, 0   }   
    };
    struct draw_options options = {
        .title = NULL,
        .subtitle = NULL,
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

        // enable gamescope overlay rendering (HIGHLY EXPERIMENTAL!!!)
        .gamescope_overlay = false,
    };

    i18n_set_info(NULL, &options);

    // don't fork to background (default)
    bool daemonize = false;

    int opt;
    while ((opt = getopt_long(argc, argv, "Gh?vqbwdilp:t:m:f:s:c:H:V:x:y:",long_options,&option_index)) != -1) {
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
        case 'p':
            i18n_set_info(optarg, &options);
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
        case 'G':
            options.gamescope_overlay = true;
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
        case 'l':
            i18n_list_presets();
            exit(EXIT_SUCCESS);
        case 'h':
#define HELP(fmtstr, ...) fprintf(stderr, "  " fmtstr "\n", ## __VA_ARGS__)
#define SECTION(name, fmtstr, ...) fprintf(stderr,(STYLE(1) "" name ": " STYLE(0) fmtstr "\n"), ## __VA_ARGS__)
#define END() fprintf(stderr, "\n")
#define STYLE(x) "\033[" # x "m"
#define COLOR(x, y) "\033[" # x ";" # y "m"
            SECTION("Usage", "%s [-b/--text-bold,-i/--text-italic,-w/--skip-compositior,-d/--daemonize,-v/--verbose,-q/--quiet] [-p, --text-preset preset] [-c, --text-color color] [-f, --text-font font] [-m, --text-message message] [-s, --scale scale] [-t, --text-title title] ...", argv[0]);
            END();

            SECTION("Text", "");
            HELP("-t, --text-title title\tSet  title  text (string)");
            HELP("-m, --text-message message\tSet message text (string)");
            HELP("-p, --text-preset preset\tSelect predefined preset (conflicts -t/-m)");
            END();

            SECTION("Appearance", "");
            HELP("-f, --text-font font\tSet the text font (string)");
            HELP("-b, --text-bold \t\tShow " STYLE(1) "bold" STYLE(0) " text");
            HELP("-i, --text-italic \t\tShow " STYLE(3) "italic/slanted" STYLE(0) " text");
            HELP("-c, --text-color color\tSpecify color in " COLOR(1, 31) "r" STYLE(0)
                 "-" COLOR(1, 32) "g" STYLE(0) "-" COLOR(1,34) "b" STYLE(0)
                 "-" COLOR(1, 33) "a" STYLE(0)  " notation");
            HELP("\t\twhere " COLOR(1, 31) "r" STYLE(0) "/" COLOR(1,32)
                 "g" STYLE(0)  "/" COLOR(1, 34) "b" STYLE(0) "/" COLOR(1, 33)
                 "a" STYLE(0) " is between " COLOR(1, 32) "0.0" STYLE(0)
                 "-" COLOR(1, 34) "1.0" STYLE(0));
            END();

            SECTION("Geometry", "");
            HELP("-x, --overlay-width width\tSet overlay width  before scaling (integer)");
            HELP("-y, --overlay-height height\tSet overlay height before scaling (integer)");
            HELP("-s, --scale scale\tScale ratio (float)");
            HELP("-H, --overlay-offset-left offset\tMove overlay horizontally (integer)");
            HELP("-V, --overlay-offset-top offset\tMove overlay  vertically  (integer)");
            END();

            SECTION("Other", "");
            HELP("-w, --skip-compositior \t\tSet EWMH bypass_compositor hint");
            HELP("-l, --text-preset-list \t\tList predefined presets");
            HELP("-d, --daemonize \t\tFork to background on startup");
            HELP("-v, --verbose \t\tBe verbose and spam console");
            HELP("-q, --quiet \t\tBe completely silent");
            HELP("-G, --gamescope \t\tRun as an external gamescope overlay (EXPERIMENTAL)");
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

#ifndef NO_WAYLAND
    // if the wayland backend fails, fallback to x11 if it was enabled.
    try_backend(wayland, &options);
#endif
#ifndef NO_X11
    try_backend(x11, &options);
#endif
}
