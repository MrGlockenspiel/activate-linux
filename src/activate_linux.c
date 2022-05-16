#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xinerama.h>

#include <cairo.h>
#include <cairo-xlib.h>

//struct to hold rgb color
struct RGBAColor
{
    //rgba color values from 0 to 1
    float r;
    float g;
    float b;
    float a;
};

// draw text
void draw(cairo_t *cr, char *title, char *subtitle, float scale, struct RGBAColor color) {
    //set color
    cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
    
    // set font size, and scale up or down
    cairo_set_font_size(cr, 24 * scale);
    cairo_move_to(cr, 20, 30 * scale);
    cairo_show_text(cr, title);
    
    cairo_set_font_size(cr, 16 * scale);
    cairo_move_to(cr, 20, 55 * scale);
    cairo_show_text(cr, subtitle);
}

//fill RGBAColor struct values from a string formatted in "r-g-b-a" from 0.0 to 1.0
void RGBAColor_from_string(struct RGBAColor* color, char* text)
{
    //split text into 4 parts along "-". If the input is not valid, use default setting
   char* red = strtok(text, "-");
   if (red != NULL)
   {
       color->r = atof(red);
   }
   char* green = strtok(NULL, "-");
   if (green != NULL)
   {
       color->g = atof(green);
   }
   char* blue = strtok(NULL, "-");
   if (green != NULL)
   {
       color->b = atof(blue);
   }
   char* alpha = strtok(NULL, "-");
   if (alpha != NULL)
   {
       color->a = atof(alpha);
   }
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

    //title and subtitle text
    char *title, *subtitle;

    int overlay_width = 340;
    int overlay_height = 120;
    
    //color of text - set default as light grey
    struct RGBAColor text_color = {.r= 1.0, .g= 1.0, .b= 1.0, .a= 0.35};

    // default scale
    float scale = 1.0f;

    // switch on arguments
    switch (argc) {
        // if there are no arguments (1 is for program name)
        case (1):
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
            break;

        // 1 argument
        case (2):
            // if argument is a number, use as scale
            if(atof(argv[1]) != 0) {
                scale = atof(argv[1]);
                #ifdef __APPLE__
                    title = "Activate MacOS";
                    subtitle = "Go to Settings to activate MacOS";
                #elif __FreeBSD__
		    title = "Activate BSD";
		    subtitle = "Go to Settings to activate BSD.";
                #else
                    title = "Activate Linux";
                    subtitle = "Go to Settings to activate Linux.";
                #endif
            }
            else {
                title = argv[1];
                subtitle = "";
            }
            break;

        // 2 arguments
        case (3):
            title = argv[1];
            subtitle = argv[2];
            break;

        // 3 arguments
        case (4):
            title = argv[1];
            subtitle = argv[2];
            scale = atof(argv[3]);
            break;

        //4 arguments
        case (5):
            title = argv[1];
            subtitle = argv[2];
            scale = atof(argv[3]);
            RGBAColor_from_string(&text_color, argv[4]);
            break;

        // if there are more than 3 arguments, print usage
        default:
            printf("More than needed arguments have been passed. This program only supports at most 4 arguments.\n");
            return 1;
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
        printf("No visual found supporting 32 bit color, terminating\n");
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
        draw(cairo_ctx[i], title, subtitle, scale, text_color);
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
