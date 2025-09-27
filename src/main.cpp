#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include "Simulator.hpp"

double last_frame_rate_update = 0;
double last_frame_time = 0;
int frame_count = 0;

void calculateFPS(GLFWwindow* window);
void onWindowRescale(GLFWwindow* window, int width, int height);
void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void onKeyPress(GLFWwindow* window, int key, int scancode, int action,
                int mods);

void loadEarthScene();
void loadMarsScene();

Renderer* renderer;
Simulator* simulator;

int main(int argc, char* argv[]) {
  if (!glfwInit()) {
    std::cerr << "GLFW didn't load and fucked everything!\n";
  }

  GLFWwindow* window =
      glfwCreateWindow(720, 480, "Gab's sumptuous balls", nullptr, nullptr);

  if (!window) {
    std::cerr << "Window didn't load and fucked everything!\n";

    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetWindowSizeCallback(window, onWindowRescale);
  glfwSetScrollCallback(window, onMouseScroll);
  glfwSetKeyCallback(window, onKeyPress);

  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW didn't load and fucked everything!\n";
  }

  renderer = new Renderer();
  simulator = new Simulator(renderer, 0);

  while (!glfwWindowShouldClose(window)) {
    simulator->update(SimulationMethod::Simple,
                      glfwGetTime() - last_frame_time);
    last_frame_time = glfwGetTime();

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    GLuint uniform_location =
        glGetUniformLocation(*renderer->getShaderProgram(), "resolution");
    glUniform2f(uniform_location, height, width);

    renderer->panning = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

    renderer->render(window);

    calculateFPS(window);
  }

  glfwTerminate();

  return 0;
}

void calculateFPS(GLFWwindow* window) {
  if ((glfwGetTime() - last_frame_rate_update) > 1.0) {
    int fps = frame_count / (glfwGetTime() - last_frame_rate_update);
    frame_count = 0;
    last_frame_rate_update = glfwGetTime();

    std::cout << fps << std::endl;
  }

  frame_count++;
}

void onWindowRescale(GLFWwindow* window, int height, int width) {
  glViewport(0, 0, height, width);
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
  renderer->processZoom(window, yoffset);
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action,
                int mods) {
  if (action != 1) {
    return;
  }

  std::cout << key;

  switch (key - 320) {
    case 1:
      loadEarthScene();
      break;
    case 2:
      loadMarsScene();
      break;
  }
}

void loadEarthScene() {
  simulator->clearBodies();
  simulator->addBody(1, 100, glm::vec2(800, 0), glm::vec3(0.55),
                     glm::vec2(0, 127));
  simulator->addBody(810, 400, glm::vec2(0, 0), glm::vec3(0.1, 0.3, 0.55),
                     glm::vec2(0, 0));
}

void loadMarsScene() {
  simulator->clearBodies();
  simulator->addBody(600, 400, glm::vec2(0, 0), glm::vec3(0.7, 0.45, 0.35),
                     glm::vec2(0, 0));
  simulator->addBody(2, 40, glm::vec2(500, 0), glm::vec3(0.7),
                     glm::vec2(0, 110));
  simulator->addBody(1, 10, glm::vec2(800, 0), glm::vec3(0.7),
                     glm::vec2(0, 88));
}
