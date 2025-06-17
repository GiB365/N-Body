#include "Renderer.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <fstream>
#include <glm/ext/vector_float2.hpp>
#include <string>
#include <vector>
#include <iostream>

Renderer::Renderer() {
  point_transforms = new std::vector<glm::vec3>;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, point_transforms->size() * sizeof(glm::vec3),
               point_transforms->data(), GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
  glEnableVertexAttribArray(0);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);

  std::string vertex_shader = shaderToString("../shaders/vertex.glsl");
  std::string fragment_shader = shaderToString("../shaders/fragment.glsl");

  const char* vertex_source = vertex_shader.c_str();
  const char* fragment_source = fragment_shader.c_str();

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_source, nullptr);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_source, nullptr);
  glCompileShader(fs);

  shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);
}

void Renderer::addCircle(float radius, glm::vec2 position) {
  point_transforms->emplace_back(glm::vec3(position.x, position.y, radius));
}

void Renderer::render(GLFWwindow* window, bool clear, bool wireframe) {
  glClearColor(0.1, 0.1, 0.12, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  glUseProgram(shader_program);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, point_transforms->size() * sizeof(glm::vec3),
               point_transforms->data(), GL_DYNAMIC_DRAW);

  glBindVertexArray(vao);

  glDrawArrays(GL_POINTS, 0, point_transforms->size());

  if (clear) {
    point_transforms->clear();
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
