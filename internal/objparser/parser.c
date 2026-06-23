#include "parser.h"
#define CGLTF_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>

#include "internal/cgltf/cgltf.h"

ModelData* parser_gltf_parse(const char* path) {
  cgltf_options options = {0};
  cgltf_data* data = NULL;

  if (cgltf_parse_file(&options, path, &data) != cgltf_result_success)
    return NULL;
  cgltf_load_buffers(&options, data, path);

  ModelData* model = malloc(sizeof(ModelData));
  cgltf_primitive* prim = &data->meshes[0].primitives[0];

  cgltf_accessor* pos_acc = NULL;
  for (size_t i = 0; i < prim->attributes_count; i++) {
    if (prim->attributes[i].type == cgltf_attribute_type_position) {
      pos_acc = prim->attributes[i].data;
      break;
    }
  }

  model->vertices_count = pos_acc->count;
  model->vertices = malloc(model->vertices_count * 3 * sizeof(float));
  cgltf_accessor_unpack_floats(pos_acc, model->vertices,
                               model->vertices_count * 3);

  cgltf_accessor* ind_acc = prim->indices;
  model->indices_count = ind_acc->count;
  model->indices = malloc(model->indices_count * sizeof(int));
  for (int i = 0; i < model->indices_count; i++) {
    model->indices[i] = (unsigned int)cgltf_accessor_read_index(ind_acc, i);
  }

  model->r = 1.0f;
  model->g = 1.0f;
  model->b = 1.0f;
  model->a = 1.0f;
  if (prim->material != NULL) {
    float* color = prim->material->pbr_metallic_roughness.base_color_factor;
    model->r = color[0];
    model->g = color[1];
    model->b = color[2];
    model->a = color[3];
  }

  free(data);
  return model;
}
