#include<time.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include "internal/window/window.h"
#include "internal/render/render.h"
#include "internal/veishik/veishik.h"

#define WIN_W 800
#define WIN_H 600

int main() {
  srand(time(NULL));

  GLFWwindow* window = window_init(WIN_W, WIN_H, "veishuya");
  if (window == NULL) {
    return -1;
  }
  printf("window initialized\n");

  render_init();
  printf("render initialized\n");

  Object obj = {
    bounds: { 0.0f, 0.0f, 0.0f, 0.2f, 0.25f },
    color: {1.0f, 0.0f, 0.0f, 1.0f}
  };

  Color bg_color = {0.0f, 0.0f, 0.0f, 0.0f};

  Bounds cam = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

  Veishik veishik;
  veishik_init(&veishik, &obj);

  float last_time = 0.0f;
  while (!window_should_close(window)) {
    float current_time = glfwGetTime();
    float delta_time = current_time - last_time;
    last_time = current_time;

    veishik_update(&veishik, delta_time);
    render_clear(&bg_color);
    render_draw(&veishik.obj, &cam);
    window_update(window);
  }

  printf("window closed\n");
  window_close(window);

  return 0;
}
