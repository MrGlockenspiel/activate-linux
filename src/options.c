#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "log.h"
#include "options.h"
#include "i18n.h"

#ifdef LIBCONFIG
  #include "config.h"
#endif

void print_help(const char* file_name);

Options options = {
  // title and subtitle takes from default preset
  // determined on compilation stage
  .title = NULL,
  .subtitle = NULL,
  .custom_font = "",

  .bold_mode = false,
  .italic_mode = false,

  .scale = 1.0f,

  // overlay position
  .overlay_width = 340,
  .overlay_height = 120,
  .offset_left = 0,
  .offset_top = 0,

  // default color is light grey, which has enough contrast to be noticed
  // on both light and dark background.
  .text_color = {.r=0.7686275, .g=0.7686275, .b=0.7686275, .a=0.4},

  // bypass compositor hint
  .bypass_compositor = false,

  // enable gamescope overlay rendering (HIGHLY EXPERIMENTAL!!!)
  .gamescope_overlay = false,

  // don't fork to background (default)
  .daemonize = false,

  // kill running instance of activate-linux
  .kill_running = false,
#ifdef X11
      .force_xshape = false,
#endif
};


void parse_options(int argc, char *const argv[]) {
  __debug__("Start option parsing\n");

  int option_index = 0;
  const struct option long_options[] = {
    // text
    {"text-title",          required_argument, NULL, 't'},
    {"text-message",        required_argument, NULL, 'm'},
    {"text-preset",         required_argument, NULL, 'p'},
    // appearance
    {"text-font",           required_argument, NULL, 'f'},
    {"text-bold",           no_argument,       NULL, 'b'},
    {"text-italic",         no_argument,       NULL, 'i'},
    {"text-color",          required_argument, NULL, 'c'},
    // size and position
    {"overlay-width",       required_argument, NULL, 'x'},
    {"overlay-height",      required_argument, NULL, 'y'},
    {"scale",               required_argument, NULL, 's'},
    {"overlay-offset-left", required_argument, NULL, 'H'},
    {"overlay-offset-top",  required_argument, NULL, 'V'},
    // other
    {"bypass-compositor",   no_argument,       NULL, 'w'},
    {"daemonize",           no_argument,       NULL, 'd'},
    {"kill-running",        no_argument,       NULL, 'K'},
    {"verbose",             no_argument,       NULL, 'v'},
    {"text-preset-list",    no_argument,       NULL, 'l'},
    {"quiet",               no_argument,       NULL, 'q'},
    {"gamescope",           no_argument,       NULL, 'G'},
#ifdef X11
    {"force-xshape",           no_argument,       NULL, 'S'},
#endif
#ifdef LIBCONFIG
    {"config-file",         required_argument, NULL, 'C'},
#endif
    {"help",                no_argument,       NULL, 'h'},
    {NULL, 0, NULL, 0},
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "t:m:p:f:bic:x:y:s:H:V:wdKvlqGh"
#ifdef X11
      "S"
#endif
#ifdef LIBCONFIG
      "C:"
#endif
      , long_options, &option_index)) != -1) {
    __debug__("Got option \"%c\" (%c) with argument \"%s\"\n", opt, optopt, optarg);
    switch (opt) {
      // text
      case 't': options.title = optarg; break;
      case 'm': options.subtitle = optarg; break;
      case 'p': i18n_set_info(optarg); break;
      // appearance
      case 'f': options.custom_font = optarg; break;
      case 'b': options.bold_mode = true; break;
      case 'i': options.italic_mode = true; break;
      // size and position
      case 'x': options.overlay_width = atoi(optarg); break;
      case 'y': options.overlay_height = atoi(optarg); break;
      case 'H': options.offset_left = atoi(optarg); break;
      case 'V': options.offset_top = atoi(optarg); break;
      // other
      case 'w': options.bypass_compositor = true; break;
      case 'd': options.daemonize = true; break;
      case 'K': options.kill_running = true; break;
      case 'v': inc_verbose(); break;
      case 'q': set_silent(); break;
      case 'G': options.gamescope_overlay = true; break;
#ifdef LIBCONFIG
      case 'C': load_config(optarg); break;
#endif
#ifdef X11
      case 'S': options.force_xshape = true; break;
#endif
      case 's':
        options.scale = atof(optarg);
        if (options.scale < 0.0f) {
          __error__("Cannot parse custom scale value. It must be number from 0.0 to 1.0\n");
          exit(EXIT_FAILURE);
        }
        break;
      case 'c':
        options.text_color = rgba_color_string(optarg);
        if (options.text_color.a < 0.0f) {
          __error__("Cannot parse custom color value. Please, use option -h to check proper format\n");
          exit(EXIT_FAILURE);
        }
        break;
      case 'l':
        i18n_list_presets();
        exit(EXIT_SUCCESS);
      case '?':
      case 'h':
        print_help(argv[0]);
        exit(EXIT_SUCCESS);
    }
  }
}

void print_help(const char *const file_name) {
#define HELP(fmtstr, ...) fprintf(stderr, "  " fmtstr "\n", ##__VA_ARGS__)
#define SECTION(name, fmtstr, ...) \
  fprintf(stderr, (STYLE(1) "" name ": " STYLE(0) fmtstr "\n"), ##__VA_ARGS__)
#define END() fprintf(stderr, "\n")
#ifdef COLOR_HELP
  #define STYLE(x) "\033[" #x "m"
  #define COLOR(x, y) "\033[" #x ";" #y "m"
#else
  #define STYLE(x)
  #define COLOR(x, y)
#endif

  SECTION(
    "Usage",
    "%s "
    "[-b/--text-bold,-i/--text-italic,-w/--skip-compositior,-d/"
    "--daemonize,-v/--verbose,-q/--quiet] [-p, --text-preset preset] "
    "[-c, --text-color color] [-f, --text-font font] [-m, --text-message "
    "message] [-s, --scale scale] [-t, --text-title title] ...",
    file_name);
  END();

  SECTION("Text", "");
  HELP("-t, --text-title title\tSet  title  text (string)");
  HELP("-m, --text-message message\tSet message text (string)");
  HELP("-p, --text-preset preset\tSelect predefined preset (conflicts "
      "-t/-m)");
  END();

  SECTION("Appearance", "");
  HELP("-f, --text-font font\t\tSet the text font (string)");
  HELP("-b, --text-bold \t\tShow " STYLE(1) "bold" STYLE(0) " text");
  HELP("-i, --text-italic \t\tShow " STYLE(3) "italic" STYLE(0) " text");
  HELP("-c, --text-color color\tSpecify color in "
      COLOR(1, 31) "r" STYLE(0) "-" COLOR(1, 32) "g" STYLE(0) "-"
      COLOR(1, 34) "b" STYLE(0) "-" COLOR(1, 33) "a" STYLE(0) " notation");
  HELP("\t\t\t\t where " COLOR(1, 31) "r" STYLE(0) "/" COLOR(1, 32) "g" STYLE(0) "/"
      COLOR(1, 34) "b" STYLE(0) "/" COLOR(1, 33) "a" STYLE(0) " is between "
      COLOR(1, 32) "0.0" STYLE(0) "-" COLOR(1, 34) "1.0" STYLE(0));
  END();

  SECTION("Geometry", "");
  HELP("-x, --overlay-width width \tSet overlay width  before scaling (integer)");
  HELP("-y, --overlay-height height \tSet overlay height before scaling (integer)");
  HELP("-s, --scale scale \t\tScale ratio (float)");
  HELP("-H, --overlay-offset-left offset \tMove overlay horizontally (integer)");
  HELP("-V, --overlay-offset-top offset \tMove overlay  vertically  (integer)");
  END();

  SECTION("Other", "");
  HELP("-w, --bypass-compositor \tSet EWMH bypass_compositor hint");
  HELP("-d, --daemonize \t\tFork to background on startup");
  HELP("-K, --kill-running \t\tKill running activate-linux instance");
  HELP("-l, --text-preset-list \tList predefined presets");
  HELP("-v, --verbose \t\tIncrease console spam level");
  HELP("-q, --quiet \t\t\tBecome completely silent");
  HELP("-G, --gamescope \t\tRun as an external gamescope overlay (EXPERIMENTAL)");
#ifdef X11
  HELP("-S, --force-xshape \t\tUse the X11 shaping extention for rendering fake transparency.");
#endif
#ifdef LIBCONFIG
  HELP("-C, --config-file \t\tLoad options from an external configuration file");
#endif

  END();
#undef HELP
#undef SECTION
#undef END
#undef STYLE
#undef COLOR
}
