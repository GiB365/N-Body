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

  // Setup points vao and vbo

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
  glBindVertexArray(0);

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

  // Setup lines vao and vbo

  glGenBuffers(1, &lines_vbo);

  glGenVertexArrays(1, &lines_vao);
  glBindVertexArray(lines_vao);
  glBindBuffer(GL_ARRAY_BUFFER, lines_vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  // Setup point 1
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

  // Setup point 2
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(2 * sizeof(float)));

  // Setup thickness
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(4 * sizeof(float)));

  // Setup color
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(5 * sizeof(float)));

  // Create line shader

  std::string line_vertex_shader_string =
      shaderToString("../shaders/line_vertex.glsl");
  std::string line_geometry_shader_string =
      shaderToString("../shaders/line_geometry.glsl");
  std::string line_fragment_shader_string =
      shaderToString("../shaders/line_fragment.glsl");

  const char* line_vertex_source = line_vertex_shader_string.c_str();
  const char* line_geometry_source = line_geometry_shader_string.c_str();
  const char* line_fragment_source = line_fragment_shader_string.c_str();

  line_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(line_vertex_shader, 1, &line_vertex_source, nullptr);
  glCompileShader(line_vertex_shader);

  line_geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(line_geometry_shader, 1, &line_geometry_source, nullptr);
  glCompileShader(line_geometry_shader);

  line_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(line_fragment_shader, 1, &line_fragment_source, nullptr);
  glCompileShader(line_fragment_shader);

  line_shader_program = glCreateProgram();
  glAttachShader(line_shader_program, line_vertex_shader);
  glAttachShader(line_shader_program, line_geometry_shader);
  glAttachShader(line_shader_program, line_fragment_shader);
  glLinkProgram(line_shader_program);

  glDeleteShader(line_vertex_shader);
  glDeleteShader(line_geometry_shader);
  glDeleteShader(line_fragment_shader);

  checkCompileStatus(point_vertex_shader, "point vertex");
  checkCompileStatus(point_fragment_shader, "point fragment");

  checkCompileStatus(line_vertex_shader, "line vertex");
  checkCompileStatus(line_geometry_shader, "line geometry");
  checkCompileStatus(line_fragment_shader, "line fragment");
}

void Renderer::addCircle(glm::vec2 position, float radius, glm::vec3 color) {
  points_data.insert(points_data.end(), {position.x, position.y, radius,
                                         color.x, color.y, color.z});
}

void Renderer::addLine(glm::vec2 point1, glm::vec2 point2, float thickness,
                       glm::vec3 color) {
  lines_data.insert(lines_data.end(), {point1.x, point1.y, point2.x, point2.y,
                                       thickness, color.x, color.y, color.z});
}

void Renderer::render(GLFWwindow* window, bool clear, bool wireframe) {
  glClearColor(0.1, 0.1, 0.12, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  processPan(window);

  int screen_width, screen_height;
  glfwGetWindowSize(window, &screen_width, &screen_height);

  // Set uniforms for line shader
  glUseProgram(line_shader_program);
  GLint uniform_location;
  uniform_location = glGetUniformLocation(line_shader_program, "zoom");
  glUniform1f(uniform_location, zoom);
  uniform_location = glGetUniformLocation(line_shader_program, "resolution");
  glUniform2f(uniform_location, screen_width, screen_height);
  uniform_location = glGetUniformLocation(line_shader_program, "offset");
  glUniform2f(uniform_location, camera_position.x, camera_position.y);

  // Draw lines
  glBindVertexArray(lines_vao);
  glBindBuffer(GL_ARRAY_BUFFER, lines_vbo);
  glBufferData(GL_ARRAY_BUFFER, lines_data.size() * sizeof(float),
               lines_data.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_POINTS, 0, lines_data.size() / 8);

  // Set uniforms for point shader
  glUseProgram(point_shader_program);
  uniform_location = glGetUniformLocation(point_shader_program, "zoom");
  glUniform1f(uniform_location, zoom);
  uniform_location = glGetUniformLocation(point_shader_program, "resolution");
  glUniform2f(uniform_location, screen_width, screen_height);
  uniform_location = glGetUniformLocation(point_shader_program, "offset");
  glUniform2f(uniform_location, camera_position.x, camera_position.y);

  // Draw points
  glBindVertexArray(points_vao);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, points_data.size() * sizeof(float),
               points_data.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_POINTS, 0, points_data.size() / 6);
  glBindVertexArray(0);

  points_data.clear();
  lines_data.clear();
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
    std::cerr << "Shader " << filename
              << " didn't load and fucked everything!\n";
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

void Renderer::processPan(GLFWwindow* window) {
  double mouse_x, mouse_y;
  glfwGetCursorPos(window, &mouse_x, &mouse_y);

  if (panning) {
    glm::vec2 mouse_delta =
        previous_mouse_position - glm::vec2(mouse_x, mouse_y);
    camera_position += glm::vec2(mouse_delta.x, -mouse_delta.y) *
                       glm::vec2(pow(zoom / 1000, 2));
  }

  previous_mouse_position = glm::vec2(mouse_x, mouse_y);
}

void checkCompileStatus(GLuint shader, const std::string& name) {
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> log(logLength);
    glGetShaderInfoLog(shader, logLength, nullptr, log.data());
    std::cerr << "ERROR: Shader compilation failed (" << name << ")\n"
              << log.data() << std::endl;
  }
}

void checkLinkStatus(GLuint program, const std::string& name) {
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> log(logLength);
    glGetProgramInfoLog(program, logLength, nullptr, log.data());
    std::cerr << "ERROR: Program linking failed (" << name << ")\n"
              << log.data() << std::endl;
  }
}

#endif
