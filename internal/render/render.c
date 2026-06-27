#define GL_GLEXT_PROTOTYPES  // for Linux features functions
#include "render.h"

#include <GL/gl.h>
#include <GL/glext.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/mat4/mat4.h"
#include "internal/objparser/parser.h"
#include "shaders.h"

static unsigned int program_id = 0;
static unsigned int VAO, VBO, EBO;
static int color_loc = -1;
static int uMVP_loc = -1;
static int uBones_loc = -1;
static int uHasBones_loc = -1;

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
  uBones_loc = glGetUniformLocation(program_id, "uBones");
  uHasBones_loc = glGetUniformLocation(program_id, "uHasBones");

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
  glBufferData(GL_ARRAY_BUFFER, obj->vertices_count * sizeof(Vertex),
               obj->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->indices_count * sizeof(int),
               obj->indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribIPointer(1, 4, GL_UNSIGNED_INT, sizeof(Vertex),
                         (void*)offsetof(Vertex, bone_indices));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, bone_weights));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  printf("buffers initialized\n");

  RenderObject res = {
      .vertices = obj->vertices,
      .indices = obj->indices,
      .indices_count = obj->indices_count,
      .color = {.r = obj->r, .g = obj->g, .b = obj->b, .a = obj->a},
      .bones = obj->bones,
      .bones_count = obj->bones_count,
  };

  free(obj);

  return res;
}

void render_draw(RenderObject* obj, Bounds* cam, float winW, float winH) {
  if (program_id == 0) return;
  glUseProgram(program_id);

  float model[16];
  float view[16];
  float projection[16];
  float tmp[16];
  float mvp[16];

  // Matrix multiplication

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

  // Bones

  if (obj->bones_count > 0 && obj->bones != NULL) {
    glUniform1i(uHasBones_loc, 1);

    float global_matrices[64 * 16];
    float bone_matrices[64 * 16];

    for (int i = 0; i < obj->bones_count && i < 64; i++) {
      int parent = obj->bones[i].parent_index;
      if (parent == -1) {
        memcpy(&global_matrices[i * 16], obj->bones[i].transform,
               16 * sizeof(float));
      } else {
        mat4_mul(&global_matrices[i * 16], &global_matrices[parent * 16],
                 obj->bones[i].transform);
      }

      mat4_mul(&bone_matrices[i * 16], &global_matrices[i * 16],
               obj->bones[i].inverse_bind);
    }

    glUniformMatrix4fv(uBones_loc, obj->bones_count, GL_FALSE, bone_matrices);
  } else {
    glUniform1i(uHasBones_loc, 0);
  }

  // Draw

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, obj->indices_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

// render_free frees RenderObject
void render_free(RenderObject* obj) {
  free(obj->vertices);
  free(obj->indices);
}

// render_clear changes the clear color
void render_clear(Color* color) {
  glClearColor(color->r, color->g, color->b, color->a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
