#include <glm/ext/vector_float2.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "Quadtree.hpp"

enum SimulationMethod { Simple, BarnesHut, FastMultipole };

class Body {
 public:
  glm::vec3 color;

  glm::vec2 position;
  glm::vec2 velocity;
  float mass;
  int radius;

  Body(float mass, int radius, glm::vec2 position, glm::vec3 color,
       glm::vec2 velocity);

  void render(Renderer* renderer);
};

class Simulator {
 private:
  std::vector<Body> bodies;
  Renderer* renderer;
  QuadTree* tree;

 public:
  Simulator(Renderer* renderer, int body_count = 0);

  void addBody(float mass, int radius, glm::vec2 position,
               glm::vec3 color = glm::vec3(1),
               glm::vec2 velocity = glm::vec2(0, 0));

  void update(SimulationMethod method, double delta);

  void simple(double delta);
  void barnesHut(double delta);
  void fastMultipole(double delta);

  void render();
};
