#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>
#include <vector>
#include <glm/vec3.hpp>
#include <string>

class Renderer {
 private:
  GLuint vao, vbo;
  GLuint shader_program;
  GLuint vs, fs;

 public:
  std::vector<glm::vec3>* point_transforms;

  Renderer();

  void addCircle(float radius, glm::vec2 position);

  void render(GLFWwindow* window, bool clear = true, bool wireframe = false);

  std::string shaderToString(const std::string& filename);
};
