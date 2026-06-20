#ifndef OBJPARSER_H
#define OBJPARSER_H

typedef struct {
  float x, y, z;
} Vertex;

typedef struct {
  unsigned int v1, v2, v3;
} Indices;

typedef struct {
  float* vertices;
  int vertices_count;
  int* indices;
  int indices_count;
} OBJModelData;

OBJModelData* parser_obj_parse(const char* path);

#endif
