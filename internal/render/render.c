#include "render.h"
#include "shaders.h"
#include<stdio.h>
#include<stdlib.h>
#include<GL/gl.h>

static unsigned int program_id;

// render_init initializes shaders
void render_init() {
  program_id = shader_init_base();
  if (program_id == 0) {
    fprintf(stderr, "unable to initialize shaders\n");
    exit(1);
  }
  printf("shaders initialized\n");
}

// render_clear changes the clear color
void render_clear(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// render_draw draws primitive shapes
void render_draw() {}
