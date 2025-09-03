#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include "Simulator.hpp"

const float ZOOM_SCALING = 1.1;

double last_frame_rate_update = 0;
double last_frame_time = 0;
int frame_count = 0;

void calculateFPS(GLFWwindow* window);
void onWindowRescale(GLFWwindow* window, int width, int height);
void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

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

  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW didn't load and fucked everything!\n";
  }

  renderer = new Renderer();
  simulator = new Simulator(renderer, 0);

  // Earth + moon
  // simulator->addBody(1, 100, glm::vec2(500, 0), glm::vec3(0.55),
  //                    glm::vec2(0, 127));
  // simulator->addBody(810, 400, glm::vec2(0, 0), glm::vec3(0.1, 0.3, 0.55),
  //                  glm::vec2(0, 0));
  //  Mars + Deimos + Phobos
  simulator->addBody(600, 400, glm::vec2(0, 0), glm::vec3(0.7, 0.45, 0.35),
                     glm::vec2(0, 0));
  simulator->addBody(2, 20, glm::vec2(500, 0), glm::vec3(0.7),
                     glm::vec2(0, 110));
  simulator->addBody(1, 10, glm::vec2(800, 0), glm::vec3(0.7),
                     glm::vec2(0, 88));

  glfwSetScrollCallback(window, onMouseScroll);

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
  int window_x, window_y;
  glfwGetWindowSize(window, &window_x, &window_y);

  double mouse_x, mouse_y;
  glfwGetCursorPos(window, &mouse_x, &mouse_y);

  renderer->zoom /= pow(ZOOM_SCALING, yoffset);

  glfwGetCursorPos(window, &mouse_x, &mouse_y);

  std::cout << "X: " << mouse_x << " Y: " << mouse_y << std::endl;
}
