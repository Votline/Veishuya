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

  // Vertices

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
  model->vertices = malloc(model->vertices_count * sizeof(Vertex));
  float* temp_pos = malloc(model->vertices_count * 3 * sizeof(float));
  cgltf_accessor_unpack_floats(pos_acc, temp_pos, model->vertices_count * 3);
  for (int i = 0; i < model->vertices_count; i++) {
    model->vertices[i].pos[0] = temp_pos[i * 3 + 0];
    model->vertices[i].pos[1] = temp_pos[i * 3 + 1];
    model->vertices[i].pos[2] = temp_pos[i * 3 + 2];
  }
  free(temp_pos);

  // Indices

  cgltf_accessor* ind_acc = prim->indices;
  model->indices_count = ind_acc->count;
  model->indices = malloc(model->indices_count * sizeof(int));
  for (int i = 0; i < model->indices_count; i++) {
    model->indices[i] = (unsigned int)cgltf_accessor_read_index(ind_acc, i);
  }

  // Color

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

  // Weights and joints

  cgltf_accessor* joints_acc = NULL;
  cgltf_accessor* weight_acc = NULL;

  for (size_t i = 0; i < prim->attributes_count; i++) {
    if (prim->attributes[i].type == cgltf_attribute_type_joints) {
      joints_acc = prim->attributes[i].data;
    }
    if (prim->attributes[i].type == cgltf_attribute_type_weights) {
      weight_acc = prim->attributes[i].data;
    }
  }

  if (joints_acc && weight_acc) {
    for (int i = 0; i < model->vertices_count; i++) {
      cgltf_accessor_read_uint(joints_acc, i, model->vertices[i].bone_indices,
                               4);
      cgltf_accessor_read_float(weight_acc, i, model->vertices[i].bone_weights,
                                4);
    }
  }

  // Bones and joints

  if (data->skins_count > 0) {
    cgltf_skin* skin = &data->skins[0];
    model->bones_count = skin->joints_count;
    model->bones = malloc(model->bones_count * sizeof(Bone));

    float* inv_bind_matrices = malloc(skin->joints_count * 16 * sizeof(float));
    cgltf_accessor_unpack_floats(skin->inverse_bind_matrices, inv_bind_matrices,
                                 skin->joints_count * 16);
    for (size_t i = 0; i < skin->joints_count; i++) {
      cgltf_node* joints_node = skin->joints[i];
      strncpy(model->bones[i].name,
              joints_node->name ? joints_node->name : "unnamed", 63);
      memcpy(model->bones[i].inverse_bind, &inv_bind_matrices[i * 16],
             16 * sizeof(float));

      model->bones[i].parent_index = -1;
      if (joints_node->parent) {
        for (size_t j = 0; j < skin->joints_count; j++) {
          if (skin->joints[j] == joints_node->parent) {
            model->bones[i].parent_index = (int)j;
            break;
          }
        }
      }
      cgltf_node_transform_local(joints_node, model->bones[i].transform);
    }
    free(inv_bind_matrices);
  }

  cgltf_free(data);
  return model;
}
