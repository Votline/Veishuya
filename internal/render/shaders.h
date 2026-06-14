#ifndef SHADERS_H
#define SHADERS_H

// shaders_init compile and link shader_type shader from shader_path
unsigned int shaders_init(const char* shader_path, unsigned int shader_type);

// shader_init_base initialize vertex and fragment shader and link them
unsigned int shader_init_base();

#endif
