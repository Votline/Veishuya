#include "render.h"
#include<GL/gl.h>

// render_init initializes shaders
void render_init() {}

// render_clear changes the clear color
void render_clear(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// render_draw draws primitive shapes
void render_draw() {}
