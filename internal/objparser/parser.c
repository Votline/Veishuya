#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/slice/slice.h"

// parser_file_read reads file from path and returns its content
static char* parser_file_read(const char* path) {
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "unable to open file %s\n", path);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* buffer = malloc(size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "unable to allocate memory\n");
    fclose(file);
    return NULL;
  }

  fread(buffer, 1, size, file);
  buffer[size] = '\0';
  fclose(file);

  return buffer;
}

// parser_obj_parse parses obj file from obj_path and fill OBJModelData
// OBJModelData contains vertices and indices
OBJModelData* parser_obj_parse(const char* obj_path) {
  char* buffer = parser_file_read(obj_path);
  if (buffer == NULL) {
    return NULL;
  }

  SliceHeader vertices = slice_new(sizeof(float), 128);
  SliceHeader indices = slice_new(sizeof(int), 128);

  char* line = strtok(buffer, "\n");
  while (line != NULL) {
    if (line[0] == 'v' && line[1] == ' ') {
      float x, y, z;
      sscanf(line, "v %f %f %f", &x, &y, &z);
      slice_append(&vertices, &x);
      slice_append(&vertices, &y);
      slice_append(&vertices, &z);
    } else if (line[0] == 'f' && line[1] == ' ') {
      int v1, v2, v3, v4;
      int parsed = sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",
                          &v1, &v2, &v3, &v4);
      if (parsed != 4 && parsed != 3) {
        parsed =
            sscanf(line, "f %d/%*d %d/%*d %d/%*d %d/%*d", &v1, &v2, &v3, &v4);
      }
      if (parsed != 4 && parsed != 3) {
        parsed = sscanf(line, "f %d//%*d %d//%*d %d//%*d %d//%*d", &v1, &v2,
                        &v3, &v4);
      }
      if (parsed != 4 && parsed != 3) {
        parsed = sscanf(line, "f %d %d %d %d", &v1, &v2, &v3, &v4);
      }
      if (parsed >= 3) {
        v1--;
        v2--;
        v3--;
        slice_append(&indices, &v1);
        slice_append(&indices, &v2);
        slice_append(&indices, &v3);

        if (parsed == 4) {
          v4--;
          slice_append(&indices, &v1);
          slice_append(&indices, &v3);
          slice_append(&indices, &v4);
        }
      }
    }
    line = strtok(NULL, "\n");
  }

  OBJModelData* res = malloc(sizeof(OBJModelData));
  res->vertices = vertices.arr;
  res->vertices_count = vertices.len;
  res->indices = indices.arr;
  res->indices_count = indices.len;

  free(buffer);
  return res;
}

MTLColor parser_mtl_parse_color(const char* mtl_path) {
  MTLColor color = {1.0f, 1.0f, 1.0f, 1.0f};

  char* buffer = parser_file_read(mtl_path);
  if (buffer == NULL) {
    return color;
  }

  char* line = strtok(buffer, "\n");
  while (line != NULL) {
    if (line[0] == 'K' && line[1] == 'd' && line[2] == ' ') {
      float r, g, b;
      if (sscanf(line, "Kd %f %f %f", &r, &g, &b) == 3) {
        color.r = r;
        color.g = g;
        color.b = b;
      }
    } else if (line[0] == 'd' && line[1] == ' ') {
      float a;
      if (sscanf(line, "d %f", &a) == 1) {
        color.a = a;
      }
    }
    line = strtok(NULL, "\n");
  }

  free(buffer);
  return color;
}
