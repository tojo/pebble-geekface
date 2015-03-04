#include <pebble.h>

static Window *s_my_window;
static Layer *s_canvas_layer;

static int16_t rows[4] = {110,88,66,44};
static int16_t columns[6] = {16,38,60,82,104,126};
static int16_t points[6] = {2,4,3,4,3,4};

static void fill_circle(GContext *ctx, GPoint p, GColor color, int size) {
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_circle(ctx, p, size);
}

static void draw_points(GContext *ctx) {
  for(unsigned int c = 0; c < 6; c++) {
    for(int r = 0; r < points[c]; r++) {
      fill_circle(ctx, GPoint(columns[c], rows[r]), GColorWhite, 8);
      fill_circle(ctx, GPoint(columns[c], rows[r]), GColorBlack, 7);
      fill_circle(ctx, GPoint(columns[c], rows[r]), GColorWhite, 5);
    }
  }
}

static void handle_canvas_update_proc(Layer *this_layer, GContext *ctx) {
  // draw background
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0,0,144,152), 0, GCornerNone);
  
  draw_points(ctx);
}

static void handle_main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create Layer
  s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, s_canvas_layer);

  // Set the update_proc
  layer_set_update_proc(s_canvas_layer, handle_canvas_update_proc);
}

static void handle_main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

void handle_init(void) {
  s_my_window = window_create();
  window_set_window_handlers(s_my_window, (WindowHandlers) {
    .load = handle_main_window_load,
    .unload = handle_main_window_unload,
  });
  window_stack_push(s_my_window, true);
}

void handle_deinit(void) {
  window_destroy(s_my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
