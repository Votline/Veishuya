#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/slice/slice.h"

// parser_obj_read reads file from path and returns its content
static char* parser_obj_read(const char* path) {
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

// parser_obj_parse parses obj file from path and fill OBJModelData
// OBJModelData contains vertices and indices
OBJModelData* parser_obj_parse(const char* path) {
  char* buffer = parser_obj_read(path);
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
