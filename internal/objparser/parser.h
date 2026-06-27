#ifndef OBJPARSER_H
#define OBJPARSER_H

typedef struct {
  float pos[3];
  unsigned int bone_indices[4];
  float bone_weights[4];
} Vertex;

typedef struct {
  char name[64];
  int parent_index;
  float transform[16];
  float inverse_bind[16];
} Bone;

typedef struct {
  unsigned int v1, v2, v3;
} Indices;

typedef struct {
  Vertex* vertices;
  int vertices_count;
  int* indices;
  int indices_count;
  float r, g, b, a;
  int bones_count;
  Bone* bones;
} ModelData;

ModelData* parser_gltf_parse(const char* path);

#endif
