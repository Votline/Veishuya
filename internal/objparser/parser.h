#ifndef OBJPARSER_H
#define OBJPARSER_H

#define UnknownType 0
#define TranslationType 1
#define RotationType 2

typedef struct {
  float pos[3];
  unsigned int bone_indices[4];
  float bone_weights[4];
} Vertex;

typedef struct {
  char name[64];
  int parent_index;
  float transform[16];
  float orig_transform[16];
  float inverse_bind[16];
} Bone;

typedef struct {
  unsigned int v1, v2, v3;
} Indices;

typedef struct {
  float time;
  float rotation[4];
  float translation[3];
} Keyframe;

typedef struct {
  int node_index;
  int kf_type;  // 1 - translation, 2 - rotation
  int keyframes_count;
  Keyframe* keyframes;
} AnimChannel;

typedef struct {
  int channels_count;
  AnimChannel* channels;
  float duration;
} AnimationData;

typedef struct {
  Vertex* vertices;
  int vertices_count;
  int* indices;
  int indices_count;
  float r, g, b, a;
  int bones_count;
  Bone* bones;
  AnimationData anim;
} ModelData;

ModelData* parser_gltf_parse(const char* path);

#endif
