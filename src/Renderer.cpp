#ifndef BODY_RENDERER
#define BODY_RENDERER

#include "Renderer.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <fstream>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>
#include <iostream>

void checkCompileStatus(GLuint shader, const std::string& name);
void checkLinkStatus(GLuint program, const std::string& name);

Renderer::Renderer() {
  zoom = 1200;
  camera_position = glm::vec2(0);

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);

  glGenBuffers(1, &points_vbo);

  glGenVertexArrays(1, &points_vao);
  glBindVertexArray(points_vao);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  // Setup position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

  // Setup radius
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(2 * sizeof(float)));

  // Setup color
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));

  // Create point shader

  std::string point_vertex_shader_string =
      shaderToString("../shaders/point_vertex.glsl");
  std::string point_fragment_shader_string =
      shaderToString("../shaders/point_fragment.glsl");

  const char* point_vertex_source = point_vertex_shader_string.c_str();
  const char* point_fragment_source = point_fragment_shader_string.c_str();

  point_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(point_vertex_shader, 1, &point_vertex_source, nullptr);
  glCompileShader(point_vertex_shader);

  point_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(point_fragment_shader, 1, &point_fragment_source, nullptr);
  glCompileShader(point_fragment_shader);

  point_shader_program = glCreateProgram();
  glAttachShader(point_shader_program, point_vertex_shader);
  glAttachShader(point_shader_program, point_fragment_shader);
  glLinkProgram(point_shader_program);

  glDeleteShader(point_vertex_shader);
  glDeleteShader(point_fragment_shader);
}

void Renderer::addCircle(glm::vec2 position, float radius, glm::vec3 color) {
  points_data.insert(points_data.end(), {position.x, position.y, radius,
                                         color.x, color.y, color.z});
}

void Renderer::render(GLFWwindow* window, bool clear, bool wireframe) {
  glClearColor(0.1, 0.1, 0.12, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  glUseProgram(point_shader_program);

  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, points_data.size() * sizeof(float),
               points_data.data(), GL_DYNAMIC_DRAW);

  glBindVertexArray(points_vao);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glDrawArrays(GL_POINTS, 0, points_data.size() / 6);
  glBindVertexArray(0);

  points_data.clear();
  glfwSwapBuffers(window);
}

std::string Renderer::shaderToString(const std::string& filename) {
  std::string result = "";

  std::string line = "";
  std::ifstream shader_file(filename.c_str());

  if (shader_file.is_open()) {
    while (std::getline(shader_file, line)) {
      result += line + '\n';
    }
    shader_file.close();
  } else {
    std::cerr << "Shaders didn't load and fucked everything!";
  }

  return result;
}

GLuint* Renderer::getShaderProgram() { return &point_shader_program; }

glm::vec2 Renderer::getGlobalMousePosition(GLFWwindow* window) {
  int window_x, window_y;
  glfwGetWindowSize(window, &window_x, &window_y);

  double mouse_x, mouse_y;
  glfwGetCursorPos(window, &mouse_x, &mouse_y);
  glm::vec2 mouse_pos = glm::vec2(mouse_x, -mouse_y) + glm::vec2(0, window_y);

  glm::vec2 global_mouse_position =
      glm::vec2(mouse_pos.x / window_x, mouse_pos.y / window_y);
  global_mouse_position *= zoom;
  global_mouse_position *= glm::vec2(2.0);
  global_mouse_position -= glm::vec2(zoom);
  global_mouse_position.y *= (float)window_y / window_x;

  return global_mouse_position;
}

void Renderer::processZoom(GLFWwindow* window, double yoffset) {
  glm::vec2 old_global_mouse_position = getGlobalMousePosition(window);

  if (yoffset < 0) {
    zoom *= ZOOM_SCALING;
  } else {
    zoom /= ZOOM_SCALING;
  }

  glm::vec2 new_global_mouse_position = getGlobalMousePosition(window);

  camera_position += old_global_mouse_position - new_global_mouse_position;
}

#endif
