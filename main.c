#include<stdio.h>
#include "internal/window/window.h"
#include "internal/render/render.h"

int main() {
  GLFWwindow* window = window_init(800, 600, "veishuya");
 
  if (window == NULL) {
    return -1;
  }

  printf("window initialized\n");

  render_init();

  printf("render initialized\n");

  Object obj = {
    bounds: { 0.0f, 0.0f, 0.2f, 0.25f },
    color: {1.0f, 0.0f, 0.0f, 1.0f}
  };

  Color bg_color = {0.0f, 0.0f, 0.0f, 0.0f};

  while (!window_should_close(window)) {
    render_clear(&bg_color);
    render_draw(&obj);
    window_update(window);
  }

  printf("window closed\n");
  window_close(window);

  return 0;
}
