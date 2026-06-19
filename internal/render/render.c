#define GL_GLEXT_PROTOTYPES // for Linux features functions
#include "render.h"
#include "shaders.h"
#include "internal/objparser/parser.h"
#include<stdio.h>
#include<stdlib.h>
#include<GL/gl.h>
#include<GL/glext.h>

static unsigned int program_id = 0;
static unsigned int VAO, VBO, EBO;
static int color_loc = -1;
static int tranf_loc = -1;
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
  tranf_loc = glGetUniformLocation(program_id, "uTransform");
  cam_loc = glGetUniformLocation(program_id, "uCamPos");

  if (color_loc == -1 || tranf_loc == -1 || cam_loc == -1) {
    fprintf(stderr, "unable to get shaders locations\n");
    exit(1);
  }
}

// render_create_model creates model from obj file
// OBJModelData contains vertices and indices
// It creates VAO, VBO and EBO and fill them
RenderObject render_create_model(const char* model_path) {
  OBJModelData* obj = parser_obj_parse(model_path);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, obj->vertices_count*sizeof(float), obj->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->indices_count*sizeof(int), obj->indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  printf("buffers initialized\n");

  RenderObject res = {
    .vertices = obj->vertices,
    .indices = obj->indices,
    .indices_count = obj->indices_count
  };

  return res;
}

// render_clear changes the clear color
void render_clear(Color* color) {
  glClearColor(color->r, color->g, color->b, color->a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// render_draw draws model hitbox
void render_draw(RenderObject* obj, Bounds* cam) {
  if (program_id == 0) {
    fprintf(stderr, "shaders not initialized\n");
    return;
  }

  glUseProgram(program_id);

  glUniform4f(color_loc, obj->color.r, obj->color.g, obj->color.b, obj->color.a);
  glUniform4f(tranf_loc, obj->bounds.x, obj->bounds.y, obj->bounds.w, obj->bounds.h);
  glUniform3f(cam_loc, cam->x, cam->y, cam->z);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, obj->indices_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

// render_free frees RenderObject
void render_free(RenderObject* obj) {
  free(obj->vertices);
  free(obj->indices);
}
