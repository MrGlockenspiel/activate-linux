// heavily based on https://github.com/swaywm/swaybg/blob/master/main.c
#define _POSIX_C_SOURCE 200112L
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/input-event-codes.h>
#include <wayland-client.h>

#include <cairo/cairo.h>

#include "wlr-layer-shell-unstable-v1.h"

#include "wayland.h"
#include "../cairo_draw_text.h"
#include "../options.h"
#include "../log.h"
#ifdef LIBCONFIG
  #include "../config.h"
#endif

#define UNUSED(expr) do { (void)(expr); } while (0)

struct state {
    struct wl_display *display;

    struct wl_compositor *compositor;
    struct wl_shm *shm;
    struct zwlr_layer_shell_v1 *layer_shell;
    struct wl_seat *seat;
    struct wl_pointer *pointer;

    struct wl_list outputs;
    struct output *pointer_output;
    struct output *drag_output;
    wl_fixed_t pointer_x;
    wl_fixed_t pointer_y;
    bool drag_active;
};

struct output {
    struct wl_list link;
    struct state *state;

    int32_t scale;
    uint32_t wl_name;

    struct wl_output *wl_output;
    struct wl_surface *surface;
    struct zwlr_layer_surface_v1 *layer_surface;

    // dimensions of the layer_surface, not the output
    uint32_t width, height;

    int32_t margin_top;
    int32_t margin_right;
    int32_t margin_bottom;
    int32_t margin_left;
};

static struct output *output_find_by_surface(struct state *state, struct wl_surface *surface)
{
    struct output *output = NULL;
    wl_list_for_each(output, &state->outputs, link) {
        if (output->surface == surface) {
            return output;
        }
    }

    return NULL;
}

static void output_apply_margin(struct output *output)
{
    if (output == NULL || output->layer_surface == NULL) {
        return;
    }

    zwlr_layer_surface_v1_set_margin(output->layer_surface,
                                     output->margin_top,
                                     output->margin_right,
                                     output->margin_bottom,
                                     output->margin_left);
    wl_surface_commit(output->surface);
}

static void randname(char *buf)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A'+(r&15)+(r&16)*2;
        r >>= 5;
    }
}

// Linux provides syscalls to do this for us, but for the interest in broader
// UNIX compatibility, we're going to open a virtual file on the filesystem.
static int anonymous_shm_open(void)
{
    char name[] = "/activate-linux-XXXXXX";
    int retries = 100;

    do {
        randname(name + strlen(name) - 6);

        --retries;
        // shm_open guarantees that O_CLOEXEC is set
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);

    return -1;
}

// renders a frame then commits
static void frame_commit(struct output *output)
{
    if (output->width == 0 || output->height == 0) {
        return;
    }

    __debug__("Rendering a wayland frame\n");

    int width = output->width * options.scale * output->scale;
    int height = output->height * options.scale * output->scale;

    int32_t stride = width * 4;
    size_t size = stride * height;

    int fd = anonymous_shm_open();
    switch(ftruncate(fd, size)) {
    case EINTR:
        __error__("! Signal caught during frame rendering.\n");
        break;
    case EINVAL:
        __error__("! Can't truncate to negative length.\n");
        break;
    case EFBIG:
        __error__("! Length is bigger than the allowed value.\n");
        break;
    case EIO:
        __error__("! I/O error during frame rendering.\n");
        break;
    case EBADF:
        __error__("! ftruncate called with bad FD.\n");
        break;
    }
    void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    struct wl_shm_pool *pool = wl_shm_create_pool(output->state->shm, fd, size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0,
                               width, height, stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    cairo_surface_t *surface = cairo_image_surface_create_for_data(data,
                               CAIRO_FORMAT_ARGB32, width, height, stride);
    cairo_t *cairo = cairo_create(surface);

    float orig_scale = options.scale;
    options.scale *= output->scale;
    draw_text(cairo, 0);
    options.scale = orig_scale;

    wl_surface_set_buffer_scale(output->surface, output->scale);
    wl_surface_attach(output->surface, buffer, 0, 0);
    wl_surface_damage_buffer(output->surface, 0, 0, INT32_MAX, INT32_MAX);
    wl_surface_commit(output->surface);

    cairo_destroy(cairo);
    cairo_surface_destroy(surface);
    wl_buffer_destroy(buffer);
    munmap(data, size);
}

static void output_destroy(struct output *output)
{
    __debug__("Destroying output\n");

    if (output->state->pointer_output == output) {
        output->state->pointer_output = NULL;
    }
    if (output->state->drag_output == output) {
        output->state->drag_output = NULL;
        output->state->drag_active = false;
    }

    if (output->layer_surface) {
        zwlr_layer_surface_v1_destroy(output->layer_surface);
    }
    if (output->wl_output) {
        wl_output_destroy(output->wl_output);
    }
    if (output->surface) {
        wl_surface_destroy(output->surface);
    }

    wl_list_remove(&output->link);
    free(output);
}

static void layer_surface_configure(void *data,
                                    struct zwlr_layer_surface_v1 *surface,
                                    uint32_t serial, uint32_t width, uint32_t height)
{
    UNUSED(surface);
    struct output *output = data;

    __debug__("Handling wayland surface configure\n");
    zwlr_layer_surface_v1_ack_configure(output->layer_surface, serial);

    // ignore same size configures
    if (output->width == width && output->height == height) {
        wl_surface_commit(output->surface);
    } else {
        output->width = width;
        output->height = height;

        frame_commit(output);
    }
}

static void layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface)
{
    UNUSED(surface);
    struct output *output = data;
    output_destroy(output);
}

static const struct zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_configure,
    .closed = layer_surface_closed,
};

static void output_geometry(void *data, struct wl_output *output, int32_t x,
                            int32_t y, int32_t width_mm, int32_t height_mm, int32_t subpixel,
                            const char *make, const char *model, int32_t transform)
{
    UNUSED(data);
    UNUSED(output);
    UNUSED(x);
    UNUSED(y);
    UNUSED(width_mm);
    UNUSED(height_mm);
    UNUSED(subpixel);
    UNUSED(make);
    UNUSED(model);
    UNUSED(transform);
}

static void output_mode(void *data, struct wl_output *output, uint32_t flags,
                        int32_t width, int32_t height, int32_t refresh)
{
    UNUSED(data);
    UNUSED(output);
    UNUSED(flags);
    UNUSED(width);
    UNUSED(height);
    UNUSED(refresh);
}

static void output_done(void *data, struct wl_output *wl_output)
{
    UNUSED(wl_output);

    struct output *output = data;

    if (!output->layer_surface) {
        output->surface = wl_compositor_create_surface(output->state->compositor);

        if (!options.wayland_draggable) {
            // Empty input region for click-through behavior.
            struct wl_region *input_region =
                wl_compositor_create_region(output->state->compositor);
            wl_surface_set_input_region(output->surface, input_region);
            wl_region_destroy(input_region);
        }

        output->layer_surface = zwlr_layer_shell_v1_get_layer_surface(
                                    output->state->layer_shell, output->surface, output->wl_output,
                                    ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY, "activate notification");

        zwlr_layer_surface_v1_set_size(output->layer_surface,
                                       options.overlay_width * options.scale,
                                       options.overlay_height * options.scale);
        zwlr_layer_surface_v1_set_anchor(output->layer_surface,
                                         ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM |
                                         ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
        zwlr_layer_surface_v1_set_exclusive_zone(output->layer_surface, -1);
        output->margin_top = 0;
        output->margin_right = -options.overlay_offset_left;
        output->margin_bottom = -options.overlay_offset_top;
        output->margin_left = 0;
        output_apply_margin(output);
        zwlr_layer_surface_v1_add_listener(output->layer_surface,
                                           &layer_surface_listener, output);
        wl_surface_commit(output->surface);
    }

    frame_commit(output);
}

static void output_scale(void *data, struct wl_output *wl_output, int32_t scale)
{
    UNUSED(wl_output);
    struct output *output = data;
    output->scale = scale;
}

// We need all these empty handlers because libwayland will complain if one of
// them is NULL
static const struct wl_output_listener output_listener = {
    .geometry = output_geometry,
    .mode = output_mode,
    .done = output_done,
    .scale = output_scale,
};

static void pointer_handle_enter(void *data, struct wl_pointer *pointer, uint32_t serial,
                                 struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    UNUSED(pointer);
    UNUSED(serial);

    struct state *state = data;
    state->pointer_output = output_find_by_surface(state, surface);
    state->pointer_x = surface_x;
    state->pointer_y = surface_y;
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer, uint32_t serial,
                                 struct wl_surface *surface)
{
    UNUSED(pointer);
    UNUSED(serial);

    struct state *state = data;
    if (state->pointer_output != NULL && state->pointer_output->surface == surface) {
        state->pointer_output = NULL;
    }
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer, uint32_t time,
                                  wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    UNUSED(pointer);
    UNUSED(time);

    struct state *state = data;
    int32_t prev_x = wl_fixed_to_int(state->pointer_x);
    int32_t prev_y = wl_fixed_to_int(state->pointer_y);
    int32_t curr_x = wl_fixed_to_int(surface_x);
    int32_t curr_y = wl_fixed_to_int(surface_y);
    state->pointer_x = surface_x;
    state->pointer_y = surface_y;

    if (!state->drag_active || state->drag_output == NULL) {
        return;
    }

    int32_t dx = curr_x - prev_x;
    int32_t dy = curr_y - prev_y;
    if (dx == 0 && dy == 0) {
        return;
    }

    state->drag_output->margin_right -= dx;
    state->drag_output->margin_bottom -= dy;
    options.overlay_offset_left = -state->drag_output->margin_right;
    options.overlay_offset_top = -state->drag_output->margin_bottom;
    output_apply_margin(state->drag_output);
}

static void pointer_handle_button(void *data, struct wl_pointer *pointer, uint32_t serial,
                                  uint32_t time, uint32_t button, uint32_t button_state)
{
    UNUSED(pointer);
    UNUSED(serial);
    UNUSED(time);

    struct state *state = data;
    if (button != BTN_LEFT) {
        return;
    }

    if (button_state == WL_POINTER_BUTTON_STATE_PRESSED) {
        if (state->pointer_output != NULL) {
            state->drag_active = true;
            state->drag_output = state->pointer_output;
        }
    } else if (button_state == WL_POINTER_BUTTON_STATE_RELEASED) {
#ifdef LIBCONFIG
        if (state->drag_output != NULL) {
            save_overlay_offsets(options.overlay_offset_left, options.overlay_offset_top);
        }
#endif
        state->drag_active = false;
        state->drag_output = NULL;
    }
}

static void pointer_handle_axis(void *data, struct wl_pointer *pointer, uint32_t time,
                                uint32_t axis, wl_fixed_t value)
{
    UNUSED(data);
    UNUSED(pointer);
    UNUSED(time);
    UNUSED(axis);
    UNUSED(value);
}

static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_handle_enter,
    .leave = pointer_handle_leave,
    .motion = pointer_handle_motion,
    .button = pointer_handle_button,
    .axis = pointer_handle_axis,
};

static void seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t capabilities)
{
    struct state *state = data;
    bool has_pointer = (capabilities & WL_SEAT_CAPABILITY_POINTER) != 0;

    if (has_pointer && state->pointer == NULL && options.wayland_draggable) {
        state->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(state->pointer, &pointer_listener, state);
    } else if (!has_pointer && state->pointer != NULL) {
        wl_pointer_destroy(state->pointer);
        state->pointer = NULL;
        state->pointer_output = NULL;
        state->drag_output = NULL;
        state->drag_active = false;
    }
}

static void seat_handle_name(void *data, struct wl_seat *seat, const char *name)
{
    UNUSED(data);
    UNUSED(seat);
    UNUSED(name);
}

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
    .name = seat_handle_name,
};

static void handle_global(void *data, struct wl_registry *registry,
                          uint32_t name, const char *interface, uint32_t version)
{
    UNUSED(version);

    __debug__("Hadling wayland global: %s\n", interface);
    struct state *state = data;
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        state->compositor =
            wl_registry_bind(registry, name, &wl_compositor_interface, 4);
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        state->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, wl_output_interface.name) == 0) {
        struct output *output = calloc(1, sizeof(struct output));
        output->state = state;
        output->wl_name = name;
        output->wl_output = wl_registry_bind(registry, name, &wl_output_interface, 2);
        wl_output_add_listener(output->wl_output, &output_listener, output);
        wl_list_insert(&state->outputs, &output->link);
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        state->seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(state->seat, &seat_listener, state);
    } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
        state->layer_shell =
            wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, 1);
    }
}

static void handle_global_remove(void *data, struct wl_registry *registry,
                                 uint32_t name)
{
    UNUSED(registry);
    __debug__("Hadling wayland global remove\n");
    struct state *state = data;
    struct output *output, *tmp;
    wl_list_for_each_safe(output, tmp, &state->outputs, link) {
        if (output->wl_name == name) {
            output_destroy(output);
            break;
        }
    }
}

static const struct wl_registry_listener registry_listener = {
    .global = handle_global,
    .global_remove = handle_global_remove,
};

int wayland_backend_start(void)
{
    struct state state = {0};

    wl_list_init(&state.outputs);

    state.display = wl_display_connect(NULL);
    if (!state.display) {
        return 1;
    }

    struct wl_registry *registry = wl_display_get_registry(state.display);
    wl_registry_add_listener(registry, &registry_listener, &state);
    if (wl_display_roundtrip(state.display) < 0) {
        __error__("Failed to roundtrip wayland display\n");
        return 1;
    }
    if (state.compositor == NULL || state.shm == NULL ||
        state.layer_shell == NULL) {
        __error__("Missing a required wayland interface\n");
        return 1;
    }

    while (wl_display_dispatch(state.display) != -1) {

    }

    if (state.pointer != NULL) {
        wl_pointer_destroy(state.pointer);
    }
    if (state.seat != NULL) {
        wl_seat_destroy(state.seat);
    }
    wl_display_disconnect(state.display);

    return 0;
}

int wayland_backend_kill_running(void) {
    __error__("wayland_backend_kill_running currently is not implemented\n");
    return 1;
}
