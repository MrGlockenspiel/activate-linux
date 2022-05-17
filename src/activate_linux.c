#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xinerama.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include "color.h"

// draw text
void draw(cairo_t *cr, char *title, char *subtitle, float scale, struct rgba_color_t color, char* customfont, int boldmode, int slantmode) {
    // set color
    cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);

    // no subpixel anti-aliasing because we are on transparent BG
    cairo_font_options_t* font_options = cairo_font_options_create();
    cairo_font_options_set_antialias(font_options, CAIRO_ANTIALIAS_GRAY);
    cairo_set_font_options(cr, font_options);

    // set font size, and scale up or down
    cairo_set_font_size(cr, 24 * scale);
    
    // font weight and slant settings 
    cairo_font_weight_t font_weight = CAIRO_FONT_WEIGHT_NORMAL;
    if (boldmode >= 1) {
	font_weight = CAIRO_FONT_WEIGHT_BOLD;
    }
	
    cairo_font_slant_t font_slant = CAIRO_FONT_SLANT_NORMAL;
    if (slantmode >= 1) {
	font_slant = CAIRO_FONT_SLANT_ITALIC;
    }

	
    cairo_select_font_face(cr, customfont, font_slant, font_weight);

    cairo_move_to(cr, 20, 30 * scale);
    cairo_show_text(cr, title);
    
    cairo_set_font_size(cr, 16 * scale);
    cairo_move_to(cr, 20, 55 * scale);
    cairo_show_text(cr, subtitle);
    
    cairo_font_options_destroy(font_options);
}

int main(int argc, char *argv[]) {
    Display *d = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(d);
    int default_screen = XDefaultScreen(d);

    int num_entries = 0;
	
    // get all screens in use
    XineramaScreenInfo *si = XineramaQueryScreens(d, &num_entries);

    // if xinerama fails
    if (si == NULL) {
        perror("Required X extension Xinerama is not active");
        XCloseDisplay(d);
        return 1;
    }

    // title, subtitle text;
    char *title, *subtitle;
    char *customfont = "";
    #ifdef __APPLE__
        title = "Activate macOS";
        subtitle = "Go to Settings to activate macOS.";
    #elif __FreeBSD__
		title = "Activate BSD";
		subtitle = "Go to Settings to activate BSD.";
    #else
        title = "Activate Linux";
        subtitle = "Go to Settings to activate Linux.";
    #endif

    int overlay_width = 340;
    int overlay_height = 120;
	
    int boldmode = 0, slantmode = 0;
	
    // color of text - set default as light grey
    struct rgba_color_t text_color = rgba_color_default();

    // default scale
    float scale = 1.0f;

    int opt;
    while ((opt = getopt(argc, argv, "?bit:m:s:f:c:")) != -1) {
           switch (opt) {
           case 'b':
               boldmode = 1;
               break;
           case 'i':
               slantmode = 1;
               break;
		   case 't':
               title = optarg;
               break;
		   case 'm':
               subtitle = optarg;
               break;
		   case 'f':
               customfont = optarg;
               break;
		   case 's':
               scale = atof(optarg);
               break;
		   case 'c':
                text_color = rgba_color_string(optarg);
				if (text_color.a < 0.0) {
					fprintf(stderr, "Error occurred during parsing custom color.\n");
					return 1;
				}
               break;  	
           case '?':
               fprintf(stderr, "Usage: %s [-b] [-c color] [-f font (string)] [-i] [-m message (string)] [-s scale (float)] [-t title (string)]\n", argv[0]);
               exit(EXIT_SUCCESS);
           }
    }

    XSetWindowAttributes attrs;
    attrs.override_redirect = 1;

    XVisualInfo vinfo;

    // MacOS doesnt support 32 bit color through XQuartz, massive hack
    #ifdef __APPLE__
        int colorDepth = 24;
    #else
        int colorDepth = 32;
    #endif
    
    if (!XMatchVisualInfo(d, default_screen, colorDepth, TrueColor, &vinfo)) {
        printf("No visuals found supporting %i bit color, terminating\n", colorDepth);
        exit(EXIT_FAILURE);
    }

    // sets 32 bit color depth
    attrs.colormap = XCreateColormap(d, root, vinfo.visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;

    Window overlay[num_entries];
    cairo_surface_t *surface[num_entries];
    cairo_t *cairo_ctx[num_entries];

    overlay_height *= scale;
    overlay_width *= scale;
    
    // create overlay on each screen
    for (int i = 0; i < num_entries; i++) {
        overlay[i] = XCreateWindow(
            d,                                                                     // display
            root,                                                                  // parent
            si[i].x_org + si[i].width - overlay_width,                             // x position
            si[i].y_org + si[i].height - overlay_height,                           // y position
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

        // cairo context
        surface[i] = cairo_xlib_surface_create(d, overlay[i], vinfo.visual, overlay_width, overlay_height);
        cairo_ctx[i] = cairo_create(surface[i]);
        
        draw(cairo_ctx[i], title, subtitle, scale, text_color, customfont, boldmode, slantmode);
    }

    // wait for X events forever
    XEvent event;
    while(1) {
        XNextEvent(d, &event);
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
