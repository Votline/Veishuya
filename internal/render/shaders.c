#define GL_GLEXT_PROTOTYPES  // for Linux features functions
#include "shaders.h"

#include <GL/gl.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>

// read_file read file from path and return its content
// allocate memory for the content
static char* read_file(const char* path) {
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

// compile_shader compile shader_type shader from shader_path
static unsigned int compile_shader(const char* shader_path,
                                   unsigned int shader_type) {
  char* shader_code = read_file(shader_path);
  if (shader_code == NULL) {
    return 0;
  }

  unsigned int shader_id = glCreateShader(shader_type);
  glShaderSource(shader_id, 1, (const char* const*)&shader_code, NULL);
  glCompileShader(shader_id);

  int success;
  char info_log[512];

  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_id, 512, NULL, info_log);
    fprintf(stderr, "Compile shader error\n%s\n", info_log);
    free(shader_code);
    return 0;
  }

  free(shader_code);
  return shader_id;
}

// shader_init initialize and link shader_type shader from shader_path
unsigned int shader_init(const char* shader_path, unsigned int shader_type,
                         unsigned int program_id) {
  unsigned int shader = compile_shader(shader_path, shader_type);

  if (shader == 0) {
    return 0;
  }

  glAttachShader(program_id, shader);
  glLinkProgram(program_id);

  int success;
  char info_log[512];

  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_id, 512, NULL, info_log);
    fprintf(stderr, "Link shader error\n%s\n", info_log);
    glDeleteProgram(program_id);
    return 0;
  }

  glDeleteShader(shader);
  return program_id;
}

// shader_init_base initialize vertex and fragment shader and link them
unsigned int shader_init_base() {
  unsigned int program_id = glCreateProgram();
  if (program_id == 0) return 0;

  program_id = shader_init("shaders/vertex.glsl", GL_VERTEX_SHADER, program_id);
  if (program_id == 0) return 0;
  program_id =
      shader_init("shaders/fragment.glsl", GL_FRAGMENT_SHADER, program_id);
  if (program_id == 0) return 0;

  return program_id;
}
