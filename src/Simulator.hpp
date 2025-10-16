#ifndef NBODY_SIMULATOR
#define NBODY_SIMULATOR

#include <glm/ext/vector_float2.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "Quadtree.hpp"
#include "Renderer.hpp"

enum SimulationMethod { Simple, BarnesHut, FastMultipole };

class Body {
 public:
  glm::vec3 color;
  std::string name;

  glm::vec2 position;
  glm::vec2 velocity;
  float mass;
  int radius;

  std::vector<glm::vec2> trail_points;
  int max_trail_length;

  Body(std::string name, float mass, int radius, glm::vec2 position,
       glm::vec3 color, glm::vec2 velocity, int max_trail_length);

  void renderBody(Renderer* renderer);
  void renderTrail(Renderer* renderer);
  void renderName(Renderer* renderer);
};

class Simulator {
 private:
  std::vector<Body> bodies;
  Renderer* renderer;
  QuadTree* tree;

 public:
  int perspective;
  float max_trail_distance;

  Simulator(Renderer* renderer, int body_count = 0);

  void addBody(float mass, int radius, glm::vec2 position,
               glm::vec3 color = glm::vec3(1),
               glm::vec2 velocity = glm::vec2(0, 0), std::string name = "",
               int max_trail_length = 100);
  void clearBodies();

  void update(SimulationMethod method, float delta, bool show_names = false,
              bool show_trails = false);

  void simple(double delta);
  void barnesHut(double delta);
  void fastMultipole(double delta);

  void changePerspective(Renderer* renderer, int new_perspective,
                         bool change_positions = true);

  void render(bool show_names, bool show_trails);
};

#endif
