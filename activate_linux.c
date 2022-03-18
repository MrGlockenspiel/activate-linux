#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

#include <cairo.h>
#include <cairo-xlib.h>

int overlay_width = 340;
int overlay_height = 120;

void draw(cairo_t *cr) {
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.35);
    
    cairo_set_font_size(cr, 24);
    cairo_move_to(cr, 20, 30);
    cairo_show_text(cr, "Activate ur mom"); 
    
    cairo_set_font_size(cr, 16);
    cairo_move_to(cr, 20, 55);
    cairo_show_text(cr, "Go to Settings to activate deez nuts.");
}

int main() {
    Display *d = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(d);
    int default_screen = XDefaultScreen(d);

    XSetWindowAttributes attrs;
    attrs.override_redirect = 1;

    XVisualInfo vinfo;
    if (!XMatchVisualInfo(d, DefaultScreen(d), 32, TrueColor, &vinfo)) {
        printf("No visual found supporting 32 bit color, terminating\n");
        exit(EXIT_FAILURE);
    }

    // sets 32 bit color depth
    attrs.colormap = XCreateColormap(d, root, vinfo.visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;

    Window overlay = XCreateWindow(
        d,                                                             // display
        root,                                                          // parent
        DisplayWidth(d, DefaultScreen(d)) - overlay_width,             // x position
        DisplayHeight(d, DefaultScreen(d)) - overlay_height,           // y position
        overlay_width,                                                 // width
        overlay_height,                                                // height
        0,                                                             // border width
        vinfo.depth,                                                   // depth
        InputOutput,                                                   // class
        vinfo.visual,                                                  // visual
        CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel, // value mask
        &attrs                                                         // attributes
    );

    XMapWindow(d, overlay);
    
    // allows the mouse to click through the overlay
    XRectangle rect;
    XserverRegion region = XFixesCreateRegion(d, &rect, 1);
    XFixesSetWindowShapeRegion(d, overlay, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(d, region);

    // cairo context
    cairo_surface_t* surface = cairo_xlib_surface_create(d, overlay, vinfo.visual, overlay_width, overlay_height);
    cairo_t* cairo_ctx = cairo_create(surface);
    draw(cairo_ctx);
    
    // wait for X events forever
    XEvent event;
    while(1) {
        XNextEvent(d, &event);
    }  

    cairo_destroy(cairo_ctx);
    cairo_surface_destroy(surface);

    XUnmapWindow(d, overlay);
    XCloseDisplay(d);
    return 0;
}
