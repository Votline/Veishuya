#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

// window_init creates transparent untouchble glfw window
GLFWwindow* window_init(int width, int height, const char* title);

// window_should_close return 1 if window should close
int window_should_close(GLFWwindow* window);

// window update swap buffers and pool events
int window_update(GLFWwindow* window);

// window terminate terminated window
int window_close(GLFWwindow* window);

#endif
