#ifndef BODY_RENDERER
#define BODY_RENDERER

#include "Renderer.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>
#include <iostream>
#include "freetype/freetype.h"

void checkCompileStatus(GLuint shader, const std::string& name);
void checkLinkStatus(GLuint program, const std::string& name);

Renderer::Renderer() {
  zoom = 1200;
  camera_position = glm::vec2(0);

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Create shader programs
  createShaderProgram("../shaders/point_vertex.glsl",
                      "../shaders/point_fragment.glsl", &point_shader_program);
  createShaderProgram("../shaders/triangle_vertex.glsl",
                      "../shaders/triangle_fragment.glsl",
                      &triangle_shader_program);
  createShaderProgram("../shaders/text_vertex_shader.glsl",
                      "../shaders/text_fragment_shader.glsl",
                      &text_shader_program);

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

  // Setup triangle vbo and vao
  glGenBuffers(1, &triangle_vbo);

  glGenVertexArrays(1, &triangle_vao);
  glBindVertexArray(triangle_vao);
  glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  // Setup position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

  // Setup color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(2 * sizeof(float)));

  // Setup text vbo and vao
  glGenBuffers(1, &text_vbo);

  glGenVertexArrays(1, &text_vao);
  glBindVertexArray(text_vao);
  glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
  glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), nullptr,
               GL_DYNAMIC_DRAW);

  // Setup quad
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  glBindVertexArray(0);

  initializeFreeType();
}

void Renderer::initializeFreeType() {
  if (FT_Init_FreeType(&freetype)) {
    std::cerr << "Freetype library didn't load and fucked everything!"
              << std::endl;
  }

  if (FT_New_Face(freetype, "../fonts/timeburner-font/Timeburner-xJB8.ttf", 0,
                  &font_face)) {
    std::cerr << "Freetype font didn't load and fucked everything!"
              << std::endl;
  }

  FT_Set_Pixel_Sizes(font_face, 0, 128);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (unsigned char c = 0; c < 128; c++) {
    if (FT_Load_Char(font_face, c, FT_LOAD_RENDER)) {
      std::cerr << "Freetype character didn't load and fucked everything!"
                << std::endl;
      continue;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_face->glyph->bitmap.width,
                 font_face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 font_face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
        texture,
        glm::vec2(font_face->glyph->bitmap.width,
                  font_face->glyph->bitmap.rows),
        glm::vec2(font_face->glyph->bitmap_left, font_face->glyph->bitmap_top),
        (GLuint)font_face->glyph->advance.x};

    Characters.insert(std::pair<char, Character>(c, character));
  }
}

void Renderer::addCircle(glm::vec2 position, float radius, glm::vec3 color) {
  points_data.insert(points_data.end(), {position.x, position.y, radius,
                                         color.x, color.y, color.z});
}

void Renderer::addTriangle(glm::vec2 point1, glm::vec2 point2, glm::vec2 point3,
                           glm::vec3 color) {
  triangle_data.insert(triangle_data.end(),
                       {point1.x, point1.y, color.x, color.y, color.z, point2.x,
                        point2.y, color.x, color.y, color.z, point3.x, point3.y,
                        color.x, color.y, color.z});
}

void Renderer::addText(std::string text, glm::vec2 position, float scale,
                       bool percentage_coords, glm::vec3 color, bool centered) {
  text_data.emplace_back(
      (TextData){text, position, scale, percentage_coords, color, centered});
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
  GLint uniform_location;

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

  // Set uniforms for triangle shader
  glUseProgram(triangle_shader_program);
  uniform_location =
      glGetUniformLocation(triangle_shader_program, "resolution");
  glUniform2f(uniform_location, screen_width, screen_height);

  // Draw triangles
  glBindVertexArray(triangle_vao);
  glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo);
  glBufferData(GL_ARRAY_BUFFER, triangle_data.size() * sizeof(float),
               triangle_data.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, triangle_data.size() / 5);
  glBindVertexArray(0);

  // Set uniforms for text shader
  glUseProgram(text_shader_program);
  uniform_location = glGetUniformLocation(text_shader_program, "resolution");
  glUniform2f(uniform_location, screen_width, screen_height);

  // Draw text
  for (TextData t : text_data) {
    renderText(window, t.text, t.position, t.scale, t.percentage_coords,
               t.color, t.centered);
  }

  points_data.clear();
  triangle_data.clear();
  text_data.clear();
  glfwSwapBuffers(window);
}

void Renderer::renderText(GLFWwindow* window, std::string text,
                          glm::vec2 position, float scale,
                          bool percentage_coords, glm::vec3 color,
                          bool centered) {
  if (text.size() == 0) {
    return;
  }

  glUniform3f(glGetUniformLocation(text_shader_program, "textColor"), color.x,
              color.y, color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(text_vao);

  int screen_width, screen_height;
  glfwGetWindowSize(window, &screen_width, &screen_height);

  if (percentage_coords) {
    position.y *= screen_height / 100.0;
    position.x *= screen_width / 100.0;
    scale /= 5000;
    scale *= screen_height;
  } else {
    position.x -= camera_position.x;
    position.y -= camera_position.y;
    position.x *= screen_width / zoom;
    position.y *= screen_width / zoom;
    position.x /= 2;
    position.y /= 2;
    position += glm::vec2((float)screen_width / 2, (float)screen_height / 2);
  }

  scale /= 2.6;

  float tallest = 0.0;
  float total_width = 0.0;
  float first_bearing = Characters[text.at(0)].Bearing.x;
  float last_edge = 0.0f;
  float penX = 0.0f;

  std::string::const_iterator c;

  for (int i = 0; i < text.size(); i++) {
    char c = text.at(i);

    Character ch = Characters[c];
    float height = ch.Bearing.y;
    tallest = std::max(height, tallest);

    float w = ch.Size.x;
    float h = ch.Size.y;
    float bearingX = ch.Bearing.x;

    float x0 = penX + bearingX;
    float x1 = x0 + w;
    last_edge = x1;

    penX += (ch.Advance >> 6);
  }

  total_width = last_edge - first_bearing;

  if (scale < 0) {
    position.y += tallest * scale;
  }
  if (centered) {
    position.y -= tallest * scale * 0.5;
    position.x -= total_width * abs(scale) * 0.5;
  }

  for (c = text.begin(); c != text.end(); c++) {
    Character ch = Characters[*c];

    float xpos = position.x + ch.Bearing.x * abs(scale);
    float ypos = position.y - (ch.Size.y - ch.Bearing.y) * abs(scale);

    float w = ch.Size.x * abs(scale);
    float h = ch.Size.y * abs(scale);
    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
        {xpos + w, ypos, 1.0f, 1.0f},

        {xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
        {xpos + w, ypos + h, 1.0f, 0.0f}};

    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    position.x += (ch.Advance >> 6) * abs(scale);
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
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
    std::cerr << "Shader " << filename << " didn't load and fucked everything!"

              << std::endl;
  }

  return result;
}

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

void Renderer::processZoom(GLFWwindow* window, double yoffset,
                           bool is_default_perspective) {
  glm::vec2 old_global_mouse_position = getGlobalMousePosition(window);

  if (yoffset < 0) {
    zoom *= ZOOM_SCALING;
  } else {
    zoom /= ZOOM_SCALING;
  }

  if (!is_default_perspective) {
    return;
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

void Renderer::createShaderProgram(const std::string& vertex_shader_path,
                                   const std::string& fragment_shader_path,
                                   GLuint* shader_program) {
  std::string vertex_shader_string = shaderToString(vertex_shader_path);
  std::string fragment_shader_string = shaderToString(fragment_shader_path);

  const char* vertex_source = vertex_shader_string.c_str();
  const char* fragment_source = fragment_shader_string.c_str();

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
  glCompileShader(fragment_shader);

  *shader_program = glCreateProgram();
  glAttachShader(*shader_program, vertex_shader);
  glAttachShader(*shader_program, fragment_shader);
  glLinkProgram(*shader_program);

  checkCompileStatus(vertex_shader, vertex_shader_path);
  checkCompileStatus(fragment_shader, fragment_shader_path);
  checkLinkStatus(*shader_program, "program");

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

#endif
