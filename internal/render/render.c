#define GL_GLEXT_PROTOTYPES  // for Linux features functions
#include "render.h"

#include <GL/gl.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>

#include "internal/mat4/mat4.h"
#include "internal/objparser/parser.h"
#include "shaders.h"

static unsigned int program_id = 0;
static unsigned int VAO, VBO, EBO;
static int color_loc = -1;
static int uMVP_loc = -1;

// render_init initializes shaders
void render_init() {
  program_id = shader_init_base();
  if (program_id == 0) {
    fprintf(stderr, "unable to initialize shaders\n");
    exit(1);
  }
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  printf("shaders initialized\n");

  color_loc = glGetUniformLocation(program_id, "uColor");
  uMVP_loc = glGetUniformLocation(program_id, "uMVP");

  if (color_loc == -1 || uMVP_loc == -1) {
    fprintf(stderr, "unable to get shaders locations\n");
    exit(1);
  }
}

// render_create_model creates model from obj file
// OBJModelData contains vertices and indices
// It creates VAO, VBO and EBO and fill them
RenderObject render_create_model(const char* model_path) {
  ModelData* obj = parser_gltf_parse(model_path);
  if (obj == NULL) {
    fprintf(stderr, "unable to parse model file\n");
    exit(1);
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, obj->vertices_count * 3 * sizeof(float),
               obj->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->indices_count * sizeof(int),
               obj->indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  printf("buffers initialized\n");

  RenderObject res = {
      .vertices = obj->vertices,
      .indices = obj->indices,
      .indices_count = obj->indices_count,
      .color = {.r = obj->r, .g = obj->g, .b = obj->b, .a = obj->a},
  };

  free(obj);

  return res;
}

// render_clear changes the clear color
void render_clear(Color* color) {
  glClearColor(color->r, color->g, color->b, color->a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void render_draw(RenderObject* obj, Bounds* cam, float winW, float winH) {
  if (program_id == 0) return;
  glUseProgram(program_id);

  float model[16];
  float view[16];
  float projection[16];
  float tmp[16];
  float mvp[16];

  mat4_model(model, obj->bounds.x, obj->bounds.y, 0.0f, obj->bounds.w,
             obj->bounds.h, 1.0f);

  mat4_model(view, -cam->x, -cam->y, -cam->z, 1.0f, 1.0f, 1.0f);

  float aspect_ratio = winW / winH;
  mat4_perspective(projection, 45.0f, aspect_ratio, 0.01f, 1000.0f);

  mat4_mul(tmp, projection, view);
  mat4_mul(mvp, tmp, model);

  glUniform4f(color_loc, obj->color.r, obj->color.g, obj->color.b,
              obj->color.a);

  glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, mvp);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, obj->indices_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

// render_free frees RenderObject
void render_free(RenderObject* obj) {
  free(obj->vertices);
  free(obj->indices);
}
