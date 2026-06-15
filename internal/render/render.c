#define GL_GLEXT_PROTOTYPES // for Linux features functions
#include "render.h"
#include "shaders.h"
#include<stdio.h>
#include<stdlib.h>
#include<GL/gl.h>
#include<GL/glext.h>

static unsigned int program_id = 0;
static unsigned int VAO, VBO, EBO;
static int color_loc = -1;
static int pos_loc = -1;

// render_init initializes shaders
void render_init() {
  program_id = shader_init_base();
  if (program_id == 0) {
    fprintf(stderr, "unable to initialize shaders\n");
    exit(1);
  }

  printf("shaders initialized\n");

  color_loc = glGetUniformLocation(program_id, "uColor");
  pos_loc = glGetAttribLocation(program_id, "transform");

  if (color_loc == -1 || pos_loc == -1) {
    fprintf(stderr, "unable to get shaders locations\n");
    exit(1);
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  float vertices[] = {
    0.5f,  0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
  };
  unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  printf("buffers initialized\n");
}

// render_clear changes the clear color
void render_clear(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// render_draw draws model hitbox
void render_draw(int x, int y, int w, int h, float r, float g, float b, float a) {
  if (program_id == 0) {
    fprintf(stderr, "shaders not initialized\n");
    return;
  }

  glUseProgram(program_id);

  glUniform4f(color_loc, r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
  glVertexAttrib4f(pos_loc, x/100.0f, y/100.0f, w/100.0f, h/100.0f);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
