#ifndef RENDER_H
#define RENDER_H

#include "internal/objparser/parser.h"

typedef struct {
  float x, y, z;
  float w, h;
} Bounds;

typedef struct {
  float r, g, b, a;
} Color;

typedef struct {
  float* vertices;
  int* indices;
  int indices_count;
  Bounds bounds;
  Color color;
} RenderObject;

// render_init initialize shaders
void render_init();

// render_clear changes color to 'rgba'
void render_clear(Color* color);

// render_draw renders quad in (x;y) with (w;h) color (r;g;b;a)
void render_draw(RenderObject* obj, Bounds* cam, float winW, float winH);

// render_create_model creates buffers and fill them
RenderObject render_create_model(const char* model_path);

// render_free frees RenderObject
void render_free(RenderObject* obj);

#endif
