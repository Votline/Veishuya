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

  while (!window_should_close(window)) {
    render_clear(0.0, 0.0, 0.0, 0.0);
    render_draw(0, 0, 15, 15, 255, 0, 0, 255);
    window_update(window);
  }

  printf("window closed\n");
  window_close(window);

  return 0;
}
