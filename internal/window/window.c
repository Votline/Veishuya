// package window creates a transpatent untouchable glfw window
#include "window.h"
#include<stdio.h>

// window_init creates a transparent untouchable glfw window
GLFWwindow* window_init(int width, int height, const char* title){
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  return window;
}

// window_should_close checks if the glfw window should be closed
int window_should_close(GLFWwindow* window){
  return glfwWindowShouldClose(window);
}

// window_update swap buffers and poll events
int window_update(GLFWwindow* window){
  glfwSwapBuffers(window);
  glfwPollEvents();
  return 0;
}

// window_close closes destroys the glfw window and terminates glfw
int window_close(GLFWwindow* window){
  if (window != NULL) {
    glfwDestroyWindow(window);
  }
  glfwTerminate();
  return 0;
}
