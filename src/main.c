#include <pebble.h>

static Window *s_my_window;
static Layer *s_canvas_layer;

static int16_t rows[4] = {110,88,66,44};
static int16_t columns[6] = {16,38,60,82,104,126};
static int16_t points[6] = {2,4,3,4,3,4};
static char buffer[] = "000000";

static void fill_circle(GContext *ctx, GPoint p, GColor color, int size) {
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_circle(ctx, p, size);
}

static void draw_points(GContext *ctx) {
  for(unsigned int c = 0; c < 6; c++) {
    for(int r = 0; r < points[c]; r++) {
      fill_circle(ctx, GPoint(columns[c], rows[r]), GColorWhite, 8);
      fill_circle(ctx, GPoint(columns[c], rows[r]), GColorBlack, 7);
      if (buffer[5] == '1') {
        fill_circle(ctx, GPoint(columns[c], rows[r]), GColorWhite, 5);
      }
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

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("000000"), "%H%M%S", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("000000"), "%I%M%S", tick_time);
  }
  
  // Create main Window element and assign to pointer
  Window *temp_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(temp_window, (WindowHandlers) {
    .load = handle_main_window_load,
    .unload = handle_main_window_unload,
  });
   
  // Show the Window on the watch, with animated=false
  window_stack_remove(s_my_window, false);
  window_stack_push(temp_window, false);
  
  s_my_window = temp_window;
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

void handle_init(void) {
  // Create main Window element and assign to pointer
  s_my_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_my_window, (WindowHandlers) {
    .load = handle_main_window_load,
    .unload = handle_main_window_unload,
  });
   
  // Show the Window on the watch, with animated=true
  window_stack_push(s_my_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

void handle_deinit(void) {
  window_destroy(s_my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
