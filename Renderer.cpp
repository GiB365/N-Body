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
  point_transforms = {};
  line_transforms = {glm::vec3(0, 0, 0), glm::vec3(1000, 1000, 0)};

  zoom = 1200;
  camera_position = glm::vec2(0);

  glGenBuffers(1, &point_vbo);
  glGenBuffers(1, &line_vbo);

  glGenVertexArrays(1, &point_vao);
  glBindVertexArray(point_vao);
  glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                        (void*)(sizeof(float) * 2));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                        (void*)(sizeof(float) * 3));

  glGenVertexArrays(1, &line_vao);
  glBindVertexArray(line_vao);
  glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, nullptr);
  glEnableVertexAttribArray(1);

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);

  std::string point_vertex_shader = shaderToString("../shaders/vertex.glsl");
  std::string fragment_shader = shaderToString("../shaders/fragment.glsl");

  const char* vertex_source = point_vertex_shader.c_str();
  const char* fragment_source = fragment_shader.c_str();

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_source, nullptr);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_source, nullptr);
  glCompileShader(fs);

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vs);
  glAttachShader(shader_program, fs);
  glLinkProgram(shader_program);

  glDeleteShader(vs);
  glDeleteShader(fs);
}

void Renderer::addCircle(float radius, glm::vec2 position, glm::vec3 color) {
  point_transforms.emplace_back(glm::vec3(position.x, position.y, radius));
  point_colors.emplace_back(color);
}

void Renderer::addSquare(glm::vec2 bottom_left, glm::vec2 top_right,
                         float line_length, glm::vec3 color) {
  line_transforms.emplace_back(glm::vec3(bottom_left, line_length));
  line_transforms.emplace_back(
      glm::vec3(bottom_left.x, top_right.y, line_length));

  line_transforms.emplace_back(
      glm::vec3(bottom_left.x, top_right.y, line_length));
  line_transforms.emplace_back(glm::vec3(top_right, line_length));

  line_transforms.emplace_back(glm::vec3(top_right, line_length));
  line_transforms.emplace_back(
      glm::vec3(top_right.x, bottom_left.y, line_length));

  line_transforms.emplace_back(
      glm::vec3(top_right.x, bottom_left.y, line_length));
  line_transforms.emplace_back(glm::vec3(bottom_left, line_length));

  line_colors.emplace_back(color);
}

void Renderer::render(GLFWwindow* window, bool clear, bool wireframe) {
  glClearColor(0.1, 0.1, 0.12, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  std::vector<float> point_data;

  for (int i = 0; i < point_transforms.size(); i++) {
    point_data.emplace_back(point_transforms[i].x);
    point_data.emplace_back(point_transforms[i].y);
    point_data.emplace_back(point_transforms[i].z);
    point_data.emplace_back(point_colors[i].x);
    point_data.emplace_back(point_colors[i].y);
    point_data.emplace_back(point_colors[i].z);
  }

  GLint primitive_uniform_location =
      glGetUniformLocation(shader_program, "isLinePrimitive");
  glUniform1i(primitive_uniform_location, 0);

  GLint zoom_uniform_location = glGetUniformLocation(shader_program, "zoom");
  glUniform1f(zoom_uniform_location, zoom);

  double mouse_x, mouse_y;
  glfwGetCursorPos(window, &mouse_x, &mouse_y);

  if (panning) {
    glm::vec2 mouse_delta =
        previous_mouse_position - glm::vec2(mouse_x, mouse_y);
    camera_position +=
        glm::vec2(-mouse_delta.x / 1000.0, mouse_delta.y / 1000.0);
  }

  previous_mouse_position = glm::vec2(mouse_x, mouse_y);

  GLuint offset_uniform_location =
      glGetUniformLocation(*getShaderProgram(), "offset");
  glUniform2f(offset_uniform_location, camera_position.x, camera_position.y);

  glUseProgram(shader_program);

  glBindVertexArray(point_vao);
  glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
  glBufferData(GL_ARRAY_BUFFER, point_data.size() * sizeof(float),
               point_data.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_POINTS, 0, point_transforms.size());

  glUniform1i(primitive_uniform_location, 1);

  glBindVertexArray(line_vao);
  glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
  glBufferData(GL_ARRAY_BUFFER, line_transforms.size() * sizeof(glm::vec3),
               line_transforms.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_LINES, 0, line_transforms.size());

  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << err << std::endl;
  }

  if (clear) {
    point_transforms.clear();
    line_transforms.clear();
  }

  glfwSwapBuffers(window);
  glfwPollEvents();
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

GLuint* Renderer::getShaderProgram() { return &shader_program; }

#endif
