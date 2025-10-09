#ifndef NBODY_RENDERER
#define NBODY_RENDERER

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include "freetype/freetype.h"
#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H

struct Character {
  unsigned int TextureID;
  glm::vec2 Size;
  glm::vec2 Bearing;
  GLuint Advance;
};

class Renderer {
 private:
  GLuint points_vao, points_vbo, triangle_vao, triangle_vbo;

  const float ZOOM_SCALING = 1.1;

  std::vector<float> points_data;    // {x, y, radius, r, g, b}
  std::vector<float> triangle_data;  // {x1, y1, x2, y2, x3, y3, r, g, b}

  FT_Library freetype;
  FT_Face font_face;
  std::map<char, Character> Characters;

 public:
  GLuint point_shader_program, triangle_shader_program;

  glm::vec2 camera_position;
  glm::vec2 previous_mouse_position;
  bool panning;
  float zoom;

  Renderer();

  void addCircle(glm::vec2 position, float radius,
                 glm::vec3 color = glm::vec3(1.0));
  void addTriangle(glm::vec2 point1, glm::vec2 point2, glm::vec2 point3,
                   glm::vec3 color = glm::vec3(1.0));

  void render(GLFWwindow* window, bool clear = true, bool wireframe = false);

  std::string shaderToString(const std::string& filename);

  void createShaderProgram(const std::string& vertex_shader_path,
                           const std::string& fragment_shader_path,
                           GLuint* shader_program);

  glm::vec2 getGlobalMousePosition(GLFWwindow* window);

  void processZoom(GLFWwindow* window, double yoffset,
                   bool is_default_perspective);
  void processPan(GLFWwindow* window);
};

#endif
