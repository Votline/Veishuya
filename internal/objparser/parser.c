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
      memcpy(model->bones[i].orig_transform, model->bones[i].transform,
             16 * sizeof(float));
    }
    free(inv_bind_matrices);
  }

  // Animation

  model->anim.channels_count = 0;
  model->anim.channels = NULL;
  model->anim.duration = 0.0f;

  if (data->animations_count > 0) {
    cgltf_animation* anim = &data->animations[0];
    model->anim.channels_count = (int)anim->channels_count;
    model->anim.channels =
        malloc(model->anim.channels_count * sizeof(AnimChannel));
    cgltf_skin* skin = data->skins_count > 0 ? &data->skins[0] : NULL;

    for (cgltf_size i = 0; i < anim->channels_count; i++) {
      cgltf_animation_channel* channel = &anim->channels[i];
      AnimChannel* my_channel = &model->anim.channels[i];

      my_channel->node_index = -1;
      for (int j = 0; j < model->bones_count; j++) {
        if (skin && skin->joints[j] == channel->target_node) {
          my_channel->node_index = j;
          break;
        }
      }
      cgltf_animation_sampler* sampler = channel->sampler;
      int keys_count = (int)sampler->input->count;
      my_channel->keyframes_count = keys_count;
      my_channel->keyframes = malloc(keys_count * sizeof(Keyframe));

      float* times = malloc(keys_count * sizeof(float));
      cgltf_accessor_unpack_floats(sampler->input, times, keys_count);

      if (channel->target_path == cgltf_animation_path_type_translation) {
        my_channel->kf_type = TranslationType;
      } else if (channel->target_path == cgltf_animation_path_type_rotation) {
        my_channel->kf_type = RotationType;
      } else {
        my_channel->kf_type = UnknownType;
      }

      for (int k = 0; k < keys_count; k++) {
        my_channel->keyframes[k].time = times[k];
        if (times[k] > model->anim.duration) {
          model->anim.duration = times[k];
        }

        my_channel->keyframes[k].rotation[0] = 0.0f;
        my_channel->keyframes[k].rotation[1] = 0.0f;
        my_channel->keyframes[k].rotation[2] = 0.0f;
        my_channel->keyframes[k].rotation[3] = 1.0f;
        my_channel->keyframes[k].translation[0] = 0.0f;
        my_channel->keyframes[k].translation[1] = 0.0f;
        my_channel->keyframes[k].translation[2] = 0.0f;
      }
      free(times);

      float* values =
          malloc(sampler->output->count *
                 cgltf_num_components(sampler->output->type) * sizeof(float));
      cgltf_accessor_unpack_floats(
          sampler->output, values,
          sampler->output->count * cgltf_num_components(sampler->output->type));

      if (channel->target_path == cgltf_animation_path_type_rotation) {
        for (int k = 0; k < keys_count; k++) {
          my_channel->keyframes[k].rotation[0] = values[k * 4 + 0];
          my_channel->keyframes[k].rotation[1] = values[k * 4 + 1];
          my_channel->keyframes[k].rotation[2] = values[k * 4 + 2];
          my_channel->keyframes[k].rotation[3] = values[k * 4 + 3];
        }
      } else if (channel->target_path ==
                 cgltf_animation_path_type_translation) {
        for (int k = 0; k < keys_count; k++) {
          my_channel->keyframes[k].translation[0] = values[k * 3 + 0];
          my_channel->keyframes[k].translation[1] = values[k * 3 + 1];
          my_channel->keyframes[k].translation[2] = values[k * 3 + 2];
        }
      }
      free(values);
    }
  }

  cgltf_free(data);
  return model;
}
