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
static int cam_loc = -1;

// render_init initializes shaders
void render_init() {
  program_id = shader_init_base();
  if (program_id == 0) {
    fprintf(stderr, "unable to initialize shaders\n");
    exit(1);
  }
  glEnable(GL_DEPTH_TEST);

  printf("shaders initialized\n");

  color_loc = glGetUniformLocation(program_id, "uColor");
  pos_loc = glGetAttribLocation(program_id, "transform");
  cam_loc = glGetUniformLocation(program_id, "uCamPos");

  if (color_loc == -1 || pos_loc == -1 || cam_loc == -1) {
    fprintf(stderr, "unable to get shaders locations\n");
    exit(1);
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  float vertices[] = {
    -0.5f, -0.5f,  0.5f, // 0
     0.5f, -0.5f,  0.5f, // 1
     0.5f,  0.5f,  0.5f, // 2
    -0.5f,  0.5f,  0.5f, // 3
    -0.5f, -0.5f, -0.5f, // 4
     0.5f, -0.5f, -0.5f, // 5
     0.5f,  0.5f, -0.5f, // 6
    -0.5f,  0.5f, -0.5f  // 7
  };

  unsigned int indices[] = {
    0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 5, 4, 7, 7, 6, 5,
    4, 0, 3, 3, 7, 4, 3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4
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
void render_clear(Color* color) {
  glClearColor(color->r, color->g, color->b, color->a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// render_draw draws model hitbox
void render_draw(Object* obj, Bounds* cam) {
  if (program_id == 0) {
    fprintf(stderr, "shaders not initialized\n");
    return;
  }

  glUseProgram(program_id);

  glUniform4f(color_loc, obj->color.r, obj->color.g, obj->color.b, obj->color.a);
  glVertexAttrib4f(pos_loc, obj->bounds.x, obj->bounds.y, obj->bounds.w, obj->bounds.h);
  glUniform3f(cam_loc, cam->x, cam->y, cam->z);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
