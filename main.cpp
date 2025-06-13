#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

float* genCircleVertices(float radius, int sides);
GLuint* genCircleIndices(float radius, int sides);

int main(int argc, char* argv[]) {
  if (!glfwInit()) {
    std::cerr << "GLFW didn't load and fucked everything!\n";
  }

  GLFWwindow* window = glfwCreateWindow(
      600, 600, "Gab's beautiful n-body simulation", nullptr, nullptr);

  if (!window) {
    std::cerr << "Window didn't load and fucked everything!\n";

    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW didn't load and fucked everything!\n";
  }

  int sides;

  std::cin >> sides;

  float* vertices = genCircleVertices(0.3, sides);

  /*
  std::cout << "Indices:\n";

  for (int i = 0; i < sides * 3; i++) {
    std::cout << indices[i] << '\n';
  }

  std::cout << "Vertices:\n";

  for (int i = 0; i < sides * 3 + 3; i++) {
    std::cout << vertices[i] << '\n';
  }
  */

  std::cout << std::endl;

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, (sides + 2) * 3 * sizeof(float), vertices,
               GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  const char* vertex_shader =
      "#version 410 core\n"
      "in vec3 vp;"
      "void main() {"
      "  gl_Position = vec4( vp, 1.0 );"
      "}";

  const char* fragment_shader =
      "#version 410 core\n"
      "out vec4 frag_colour;"
      "void main() {"
      "  frag_colour = vec4( 0.5, 0.3, 0.1, 1.0 );"
      "}";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1, 0.1, 0.12, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}

float* genCircleVertices(float radius, int sides) {
  float* vertices = new float[(sides + 2) * 3];

  float angle = M_PI - (M_PI * (sides - 2) / sides);

  std::cout << angle << std::endl;

  vertices[0] = 0;
  vertices[1] = 0;
  vertices[2] = 0;

  for (int side = 0; side < sides; side++) {
    vertices[(side + 1) * 3] = radius * std::cos(angle * side);
    vertices[(side + 1) * 3 + 1] = radius * std::sin(angle * side);
    vertices[(side + 1) * 3 + 2] = 0;
  }

  vertices[(sides + 1) * 3] = radius;
  vertices[(sides + 1) * 3 + 1] = 0;
  vertices[(sides + 1) * 3 + 2] = 0;

  return vertices;
}
