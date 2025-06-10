#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(int argc, char *argv[]) {
  if (!glfwInit()) {
    std::cerr << "GLFW didn't load and fucked everything!\n";
  }

  GLFWwindow *window = glfwCreateWindow(
      720, 480, "Gab's beautiful n-body simulation", nullptr, nullptr);

  if (!window) {
    std::cerr << "Window didn't load and fucked everything!\n";

    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
