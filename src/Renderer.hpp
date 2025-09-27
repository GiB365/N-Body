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
  GLuint points_vao, points_vbo;
  GLuint point_shader_program;
  GLuint point_vertex_shader, point_fragment_shader;

  const float ZOOM_SCALING = 1.1;

 public:
  std::vector<float> points_data;

  glm::vec2 camera_position;
  glm::vec2 previous_mouse_position;
  bool panning;
  float zoom;

  Renderer();

  void addCircle(glm::vec2 position, float radius,
                 glm::vec3 color = glm::vec3(1.0));

  void render(GLFWwindow* window, bool clear = true, bool wireframe = false);

  std::string shaderToString(const std::string& filename);

  GLuint* getShaderProgram();

  glm::vec2 getGlobalMousePosition(GLFWwindow* window);
  void processZoom(GLFWwindow* window, double yoffset);
};
