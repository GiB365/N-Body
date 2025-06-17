#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include "Renderer.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char* argv[]) {
  if (!glfwInit()) {
    std::cerr << "GLFW didn't load and fucked everything!\n";
  }

  GLFWwindow* window = glfwCreateWindow(
      720, 480, "Gab's beautiful n-body simulation", nullptr, nullptr);

  if (!window) {
    std::cerr << "Window didn't load and fucked everything!\n";

    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW didn't load and fucked everything!\n";
  }

  Renderer renderer = Renderer();

  std::cout << std::endl;

  while (!glfwWindowShouldClose(window)) {
    renderer.addCircle(25, glm::vec2(0.5, 0.5));
    renderer.addCircle(20, glm::vec2(-0.5, 0.5));
    renderer.addCircle(10, glm::vec2(0, 0));

    renderer.render(window);
  }

  glfwTerminate();

  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
