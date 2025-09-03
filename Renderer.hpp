#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>

class Renderer {
 private:
  GLuint point_vao, line_vao, point_vbo, line_vbo;
  GLuint shader_program;
  GLuint vs, fs;

 public:
  std::vector<glm::vec3> point_transforms;
  std::vector<glm::vec3> point_colors;
  std::vector<glm::vec3> line_transforms;
  std::vector<glm::vec3> line_colors;

  glm::vec2 camera_position;
  glm::vec2 previous_mouse_position;
  bool panning;
  float zoom;

  Renderer();

  void addCircle(float radius, glm::vec2 position,
                 glm::vec3 color = glm::vec3(1.0));
  void addSquare(glm::vec2 first_point, glm::vec2 second_point,
                 float line_length, glm::vec3 color = glm::vec3(1.0));

  void render(GLFWwindow* window, bool clear = true, bool wireframe = false);

  std::string shaderToString(const std::string& filename);

  GLuint* getShaderProgram();
};
