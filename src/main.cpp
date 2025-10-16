#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <vector>
#include "UIElements.hpp"
#include "Simulator.hpp"

#define MERCURY_SCENE_KEY 49
#define VENUS_SCENE_KEY 50
#define EARTH_SCENE_KEY 51
#define MARS_SCENE_KEY 52
#define JUPITER_SCENE_KEY 53
#define SATURN_SCENE_KEY 54
#define URANUS_SCENE_KEY 55
#define NEPTUNE_SCENE_KEY 56

#define PERSPECTIVE_UP_KEY 65
#define PERSPECTIVE_DOWN_KEY 68
#define TOGGLE_TRAILS_KEY 84
#define TOGGLE_NAMES_KEY 78

double last_frame_rate_update = 0;
double last_frame_time = 0;
int frame_count = 0;

void calculateFPS(GLFWwindow* window);
void onWindowRescale(GLFWwindow* window, int width, int height);
void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void onKeyPress(GLFWwindow* window, int key, int scancode, int action,
                int mods);
void onMouseButton(GLFWwindow* window, int button, int action, int mods);

void loadMercuryScene();
void loadVenusScene();
void loadEarthScene();
void loadMarsScene();
void loadJupiterScene();
void loadSaturnScene();
void loadUranusScene();
void loadNeptuneScene();

void instantiateButtons();

void previousClicked();
void nextClicked();

Renderer* renderer;
Simulator* simulator;

bool show_names = false;
bool show_trails = false;

std::vector<Button> buttons;

int main(int argc, char* argv[]) {
  glfwSetErrorCallback([](int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
  });

  if (!glfwInit()) {
    std::cerr << "GLFW didn't load and fucked everything!" << std::endl;
  }

  GLFWwindow* window =
      glfwCreateWindow(720, 480, "N-Body simulation", nullptr, nullptr);

  if (!window) {
    std::cerr << "Window didn't load and fucked everything!" << std::endl;

    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetWindowSizeCallback(window, onWindowRescale);
  glfwSetScrollCallback(window, onMouseScroll);
  glfwSetKeyCallback(window, onKeyPress);
  glfwSetMouseButtonCallback(window, onMouseButton);

  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW didn't load and fucked everything!" << std::endl;
  }

  renderer = new Renderer();
  simulator = new Simulator(renderer, 0);

  instantiateButtons();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    simulator->update(SimulationMethod::Simple, glfwGetTime() - last_frame_time,
                      show_names, show_trails);
    last_frame_time = glfwGetTime();

    renderer->panning = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

    if (renderer->panning && simulator->perspective != 0) {
      simulator->changePerspective(renderer, -1, false);
    }

    for (int i = 0; i < buttons.size(); i++) {
      buttons[i].render(renderer);
    }

    renderer->render(window);

    // calculateFPS(window);
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
  renderer->processZoom(window, yoffset, simulator->perspective == 0);
}

void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
  if (button != 0 || action != 1) {
    return;
  }

  double mouse_x, mouse_y;
  glfwGetCursorPos(window, &mouse_x, &mouse_y);
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  glm::vec2 mouse_screen_position =
      glm::vec2((float)mouse_x / width, (float)mouse_y / height);

  mouse_screen_position.y = 1 - mouse_screen_position.y;

  for (int i = 0; i < buttons.size(); i++) {
    buttons[i].checkButtonClicked(mouse_screen_position);
  }
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action,
                int mods) {
  if (action != 1) {
    return;
  }

  std::cout << key << std::endl;

  switch (key) {
    case MERCURY_SCENE_KEY:
      loadMercuryScene();
      break;
    case VENUS_SCENE_KEY:
      loadVenusScene();
      break;
    case EARTH_SCENE_KEY:
      loadEarthScene();
      break;
    case MARS_SCENE_KEY:
      loadMarsScene();
      break;
    case JUPITER_SCENE_KEY:
      loadJupiterScene();
      break;
    case SATURN_SCENE_KEY:
      loadSaturnScene();
      break;
    case URANUS_SCENE_KEY:
      loadUranusScene();
      break;
    case NEPTUNE_SCENE_KEY:
      loadNeptuneScene();
      break;
    case PERSPECTIVE_UP_KEY:
      simulator->changePerspective(renderer, simulator->perspective + 1);
      break;
    case PERSPECTIVE_DOWN_KEY:
      simulator->changePerspective(renderer, simulator->perspective - 1);
      break;
    case TOGGLE_TRAILS_KEY:
      show_trails = !show_trails;
      break;
    case TOGGLE_NAMES_KEY:
      show_names = !show_names;
      break;
  }
}

void instantiateButtons() {
  buttons.emplace_back(Button("Previous", glm::vec2(10, 0), glm::vec2(20, 5),
                              previousClicked, glm::vec3(0.4),
                              glm::vec2(1, 0)));
  buttons.emplace_back(Button("Next", glm::vec2(90, 0), glm::vec2(20, 5),
                              nextClicked, glm::vec3(0.4), glm::vec2(1, 0)));
}

void previousClicked() {
  simulator->changePerspective(renderer, simulator->perspective + 1);
}

void nextClicked() {
  simulator->changePerspective(renderer, simulator->perspective - 1);
}

void loadMercuryScene() {
  simulator->clearBodies();
  simulator->addBody(100, 100, glm::vec2(0), glm::vec3(0.55), glm::vec2(0),
                     "Mercury");

  simulator->perspective = 0;
  renderer->camera_position = glm::vec2(0);
  renderer->zoom = 1200;
}

void loadVenusScene() {
  simulator->clearBodies();
  simulator->addBody(100, 300, glm::vec2(0), glm::vec3(0.8, 0.75, 0.5),
                     glm::vec2(0), "Venus");

  simulator->perspective = 0;
  renderer->camera_position = glm::vec2(0);
  renderer->zoom = 1200;
}

void loadEarthScene() {
  simulator->clearBodies();
  simulator->addBody(810, 400, glm::vec2(0, 0), glm::vec3(0.1, 0.3, 0.55),
                     glm::vec2(0), "Earth");
  simulator->addBody(1, 100, glm::vec2(800, 0), glm::vec3(0.55),
                     glm::vec2(0, 100), "Moon");

  simulator->perspective = 0;
  renderer->camera_position = glm::vec2(0);
  renderer->zoom = 1200;
}

void loadMarsScene() {
  simulator->clearBodies();
  simulator->addBody(600, 400, glm::vec2(0, 0), glm::vec3(0.7, 0.45, 0.35),
                     glm::vec2(0), "Mars");
  simulator->addBody(2, 40, glm::vec2(500, 0), glm::vec3(0.7),
                     glm::vec2(0, 110), "Phobos");
  simulator->addBody(1, 10, glm::vec2(800, 0), glm::vec3(0.7), glm::vec2(0, 88),
                     "Deimos");

  simulator->perspective = 0;
  renderer->camera_position = glm::vec2(0);
  renderer->zoom = 1200;
}

void loadJupiterScene() {
  simulator->clearBodies();
  simulator->addBody(600, 500, glm::vec2(0), glm::vec3(0.65, 0.6, 0.45));

  // TODO: Add moons

  simulator->perspective = 0;
  renderer->camera_position = glm::vec2(0);
  renderer->zoom = 1200;
}

void loadSaturnScene() {
  simulator->clearBodies();
  simulator->addBody(500, 200, glm::vec2(0), glm::vec3(0.6), glm::vec2(0, 0));
  simulator->addBody(500, 200, glm::vec2(0, 500), glm::vec3(0.9),
                     glm::vec2(150, 0));
  simulator->addBody(500, 200, glm::vec2(0, -500), glm::vec3(0.9),
                     glm::vec2(-50, 0));

  simulator->perspective = -1;
  renderer->camera_position = glm::vec2(0);
  renderer->zoom = 1200;
}

void loadUranusScene() {
  simulator->clearBodies();

  simulator->perspective = 0;
  renderer->camera_position = glm::vec2(0);
  renderer->zoom = 1200;
}

void loadNeptuneScene() {
  simulator->clearBodies();

  simulator->perspective = 0;
  renderer->camera_position = glm::vec2(0);
  renderer->zoom = 1200;
}
