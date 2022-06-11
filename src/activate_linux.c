#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xinerama.h>
#include <X11/extensions/Xrandr.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include "color.h"
#include "i18n.h"

#include "draw.h"
#include "log.h"

// check if compositor is running
bool compositor_check(Display *d, int screen) {
    char prop_name[16];
    snprintf(prop_name, 16, "_NET_WM_CM_S%d", screen);
    Atom prop_atom = XInternAtom(d, prop_name, False);
    return XGetSelectionOwner(d, prop_atom) != None;
}

int main(int argc, char *argv[]) {
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
    while ((opt = getopt(argc, argv, "h?vbwdit:m:f:s:c:H:V:")) != -1) {
        switch (opt) {
            case 'v':
                verbose_mode = true;
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
            case '?':
            case 'h':
                #define HELP(X) fprintf(stderr, "  " X "\n")
                #define STYLE(x) "\033[" # x "m"
                #define COLOR(x, y) "\033[" # x ";" # y "m"
                fprintf(stderr, "Usage: %s [-b] [-i] [-c color] [-f font] [-m message] [-s scale] [-t title]\n", argv[0]);
                HELP("-b\t\tShow " STYLE(1) "bold" STYLE(0) " text");
                HELP("-w\t\tSet EWMH bypass_compositor hint");
                HELP("-d\t\tFork to background on startup");
                HELP("-i\t\tShow " STYLE(3) "italic/slanted" STYLE(0) " text");
                HELP("-c color\tSpecify color in " COLOR(1, 31) "r" STYLE(0)
                    "-" COLOR(1, 32) "g" STYLE(0) "-" COLOR(1,34) "b" STYLE(0)
                    "-" COLOR(1, 33) "a" STYLE(0)  " notation");
                HELP("\t\twhere " COLOR(1, 31) "r" STYLE(0) "/" COLOR(1,32)
                    "g" STYLE(0)  "/" COLOR(1, 34) "b" STYLE(0) "/" COLOR(1, 33)
                    "a" STYLE(0) " is between " COLOR(1, 32) "0.0" STYLE(0)
                    "-" COLOR(1, 34) "1.0" STYLE(0));
                HELP("-f font\tSet the text font (string)");
                HELP("-t title\tSet  title  text (string)");
                HELP("-m message\tSet message text (string)");
                HELP("-s scale\tScale ratio (float)");
                HELP("-H offset\tMove overlay horizontally (integer)");
                HELP("-V offset\tMove overlay  vertically  (integer)");
                HELP("-v\t\tBe verbose and spam console");
                #undef HELP
                #undef STYLE
                #undef COLOR
                exit(EXIT_SUCCESS);
        }
    }

    verbose_printf("Verbose mode activated\n");

    if (daemonize) {
        verbose_printf("Forking to background\n");
        int pid = -1;
        pid = fork();
        if (pid > 0) exit(EXIT_SUCCESS);
        else if(pid == 0) setsid();
    }

    verbose_printf("Opening display\n");
    Display *d = XOpenDisplay(NULL);
    verbose_printf("Finding root window\n");
    Window root = DefaultRootWindow(d);
    verbose_printf("Finding default screen\n");
    int default_screen = XDefaultScreen(d);

    verbose_printf("Checking compositor\n");
    if (!compositor_check(d, XDefaultScreen(d))) {
        printf("No running compositor detected. Program may not work as intended. \n");
    }

    // https://x.org/releases/current/doc/man/man3/Xinerama.3.xhtml
    verbose_printf("Finding all screens in use using Xinerama\n");
    int num_entries = 0;
    XineramaScreenInfo *si = XineramaQueryScreens(d, &num_entries);
    // if xinerama fails
    if (si == NULL) {
        perror("Required X extension Xinerama is not active. It is needed for displaying watermark on multiple screens");
        XCloseDisplay(d);
        return 1;
    }
    verbose_printf("Found %d screen(s)\n", num_entries);

    // https://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
    verbose_printf("Initializing Xrandr\n");
    int xrr_error_base;
    int xrr_event_base;
    if (!XRRQueryExtension(d, &xrr_event_base, &xrr_error_base)) {
        perror("Required X extension Xrandr is not active. It is needed for handling screen size change (e.g. in virtual machine window)");
        XFree(si);
        XCloseDisplay(d);
        return 1;
    }
    verbose_printf("Subscribing on screen change events\n");
    XRRSelectInput(d, root, RRScreenChangeNotifyMask);

    XSetWindowAttributes attrs;
    attrs.override_redirect = 1;

    XVisualInfo vinfo;

    // MacOS doesn't support 32 bit color through XQuartz, massive hack
    #ifdef __APPLE__
        int colorDepth = 24;
    #else
        int colorDepth = 32;
    #endif

    verbose_printf("Checking default screen to be %d bit color depth\n", colorDepth);
    if (!XMatchVisualInfo(d, default_screen, colorDepth, TrueColor, &vinfo)) {
        printf("No screens supporting %i bit color found, terminating\n", colorDepth);
        exit(EXIT_FAILURE);
    }

    verbose_printf("Set %d bit color depth\n", colorDepth);
    attrs.colormap = XCreateColormap(d, root, vinfo.visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;

    Window overlay[num_entries];
    cairo_surface_t *surface[num_entries];
    cairo_t *cairo_ctx[num_entries];

    int overlay_height = options.overlay_height * options.scale;
    verbose_printf("Scaled height: %d px\n", overlay_height);
    int overlay_width = options.overlay_width * options.scale;
    verbose_printf("Scaled width:  %d px\n", overlay_width);

    for (int i = 0; i < num_entries; i++) {
        verbose_printf("Creating overlay on %d screen\n", i);
        overlay[i] = XCreateWindow(
            d,                                                                     // display
            root,                                                                  // parent
            si[i].x_org + si[i].width + options.offset_left - overlay_width,               // x position
            si[i].y_org + si[i].height + options.offset_top - overlay_height,              // y position
            overlay_width,                                                         // width
            overlay_height,                                                        // height
            0,                                                                     // border width
            vinfo.depth,                                                           // depth
            InputOutput,                                                           // class
            vinfo.visual,                                                          // visual
            CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel,         // value mask
            &attrs                                                                 // attributes
        );
        XMapWindow(d, overlay[i]);

        // allows the mouse to click through the overlay
        XRectangle rect;
        XserverRegion region = XFixesCreateRegion(d, &rect, 1);
        XFixesSetWindowShapeRegion(d, overlay[i], ShapeInput, 0, 0, region);
        XFixesDestroyRegion(d, region);

        // sets a WM_CLASS to allow the user to blacklist some effect from compositor
        XClassHint *xch = XAllocClassHint();
        xch->res_name = "activate-linux";
        xch->res_class = "activate-linux";
        XSetClassHint(d, overlay[i], xch);

        // Set _NET_WM_BYPASS_COMPOSITOR
        // https://specifications.freedesktop.org/wm-spec/wm-spec-latest.html#idm45446104333040
        if (options.bypass_compositor) {
            verbose_printf("Bypassing compositor\n");
            unsigned char data = 1;
            XChangeProperty(
                d, overlay[i],
                XInternAtom(d, "_NET_WM_BYPASS_COMPOSITOR", False),
                XA_CARDINAL, 32, PropModeReplace, &data, 1
            );
        }

        verbose_printf("Creating cairo context\n");
        surface[i] = cairo_xlib_surface_create(d, overlay[i], vinfo.visual, overlay_width, overlay_height);
        cairo_ctx[i] = cairo_create(surface[i]);

        verbose_printf("Drawing text\n");
        draw_text(cairo_ctx[i], &options);
    }

    verbose_printf("\nAll done. Going into X windows event endless loop\n\n");
    XEvent event;
    while(1) {
        XNextEvent(d, &event);
        // handle screen resize via catching Xrandr event
        if (XRRUpdateConfiguration(&event)) {
            if (event.type-xrr_event_base == RRScreenChangeNotify) {
                verbose_printf("! Got Xrandr event about screen change\n");
                verbose_printf("  Updating info about screen sizes\n");
                si = XineramaQueryScreens(d, &num_entries);
                for (int i = 0; i < num_entries; i++) {
                    verbose_printf("  Moving window on screen %d according new position\n", i);
                    XMoveWindow(
                        d,                                                        // display
                        overlay[i],                                               // window
                        si[i].x_org + si[i].width + options.offset_left - overlay_width,  // x position
                        si[i].y_org + si[i].height + options.offset_top - overlay_height  // y position
                    );
                    verbose_printf("  Redrawing text\n");
                    draw_text(cairo_ctx[i], &options);
                }
            } else {
                verbose_printf("! Got Xrandr event, type: %d (0x%X)\n", event.type-xrr_event_base, event.type-xrr_event_base);
            }
        } else {
            verbose_printf("! Got X event, type: %d (0x%X)\n", event.type, event.type);
        }
    }

    // free used resources
    for (int i = 0; i < num_entries; i++) {
        XUnmapWindow(d, overlay[i]);
        cairo_destroy(cairo_ctx[i]);
        cairo_surface_destroy(surface[i]);
    }

    XFree(si);
    XCloseDisplay(d);
    return 0;
}
