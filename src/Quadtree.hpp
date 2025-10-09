#ifndef NBODY_QUADTREE
#define NBODY_QUADTREE

#include <glm/ext/vector_float2.hpp>
#include <vector>
#include "Renderer.hpp"

class QuadTree;

class QuadNode {
 public:
  int children_index;

  float size;
  glm::vec2 center;

  float mass;
  glm::vec2 center_of_mass;

  QuadNode(float size, glm::vec2 position);
  QuadNode(QuadNode* parent, int quad_index);

  int getQuadIndex(glm::vec2 position);
  void calculate_mass(QuadTree* tree);
};

class QuadTree {
  Renderer* renderer;

 public:
  std::vector<QuadNode> nodes;

  QuadTree(Renderer* renderer);

  void addPoint(glm::vec2 position, float mass);
  void subdivide(int node_index);
  void calculate_masses();
  glm::vec2 getAcceleration(glm::vec2 position, double delta,
                            float GRAVITATIONAL_CONSTANT, float accuracy = 1.0);
  void clear(std::vector<glm::vec2> included_points = {});

  void render();
  void print();
};

#endif
