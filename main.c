#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "internal/render/render.h"
#include "internal/veishik/veishik.h"
#include "internal/window/window.h"

enum { WIN_W = 800 };
enum { WIN_H = 600 };

int main() {
  GLFWwindow* window = window_init(WIN_W, WIN_H, "veishuya");
  if (window == NULL) {
    return -1;
  }
  printf("window initialized\n");

  render_init();
  printf("render initialized\n");

  Color bg_color = {0.0f, 0.0f, 0.0f, 0.0f};

  Bounds cam = {0.0f, 0.0f, 5.0f, 0.0f, 0.0f};

  Veishik veishik = {.render_object = {.color = {1.0f, 1.0f, 1.0f, 1.0f},
                                       .bounds = {0.0f, 0.0f, 0.0f, 0.2f, 0.2f},
                                       .scale = 0.2f}};

  veishik_init(&veishik, "assets/chibi.glb");

  float last_time = 0.0f;
  while (!window_should_close(window)) {
    float current_time = (float)glfwGetTime();
    float delta_time = current_time - last_time;
    last_time = current_time;

    veishik_update(&veishik, delta_time);
    render_clear(&bg_color);
    render_update_anim(&veishik.render_object, current_time);
    render_draw(&veishik.render_object, &cam, WIN_W, WIN_H);
    window_update(window);
  }

  printf("window closed\n");
  window_close(window);
  render_free(&veishik.render_object);

  return 0;
}
