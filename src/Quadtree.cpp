#include "Quadtree.hpp"
#include <algorithm>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <stack>
#include <cmath>

QuadNode::QuadNode(float size, glm::vec2 position) {
  this->size = size;
  this->center = position;

  this->mass = 0;
  this->center_of_mass = position;
  this->children_index = 0;
}

QuadNode::QuadNode(QuadNode* parent, int quad_index) {
  this->size = parent->size / 2.0;
  this->center.x = parent->center.x + this->size * ((quad_index & 1) - 0.5);
  this->center.y = parent->center.y + this->size * (0.5 - (quad_index >> 1));

  this->mass = 0;
  this->center_of_mass = this->center;
  this->children_index = 0;
}

int QuadNode::getQuadIndex(glm::vec2 position) {
  bool x = position.x > center.x;
  bool y = position.y < center.y;

  int output = 0;

  if (x) output++;
  if (y) output += 2;

  return output;
}

void QuadNode::calculate_mass(QuadTree* tree) {
  if (children_index == 0) return;

  mass = 0;
  center_of_mass = glm::vec2(0);

  for (int i = 0; i < 4; i++) {
    tree->nodes[children_index + i].calculate_mass(tree);
    mass += tree->nodes[children_index + i].mass;
  }
}

QuadTree::QuadTree(Renderer* renderer) {
  this->renderer = renderer;
  this->clear();
}

void QuadTree::addPoint(glm::vec2 position, float mass) {
  int current_node_index = 0;

  while (nodes[current_node_index].children_index != 0) {
    int quad_index = nodes[current_node_index].getQuadIndex(position);

    current_node_index = nodes[current_node_index].children_index + quad_index;
  }

  if (nodes[current_node_index].mass == 0) {
    nodes[current_node_index].center_of_mass = position;
    nodes[current_node_index].mass = mass;
    return;
  }

  glm::vec2 old_center_of_mass = nodes[current_node_index].center_of_mass;
  float old_mass = nodes[current_node_index].mass;

  if (position == old_center_of_mass) {
    nodes[current_node_index].mass += mass;
    return;
  }

  int old_quad_index = 0;
  int quad_index = 0;

  while (old_quad_index == quad_index) {
    subdivide(current_node_index);

    old_quad_index = nodes[current_node_index].getQuadIndex(old_center_of_mass);
    quad_index = nodes[current_node_index].getQuadIndex(position);

    if (old_quad_index == quad_index) {
      current_node_index =
          nodes[current_node_index].children_index + old_quad_index;
    } else {
      QuadNode* node1 =
          &nodes[nodes[current_node_index].children_index + old_quad_index];
      QuadNode* node2 =
          &nodes[nodes[current_node_index].children_index + quad_index];

      node1->mass = old_mass;
      node1->center_of_mass = old_center_of_mass;
      node2->mass = mass;
      node2->center_of_mass = position;
    }
  }
}

void QuadTree::subdivide(int node_index) {
  nodes[node_index].children_index = nodes.size();

  for (int i = 0; i < 4; i++) {
    nodes.emplace_back(QuadNode(&nodes[node_index], i));
  }
}

void QuadTree::calculate_masses() {
  if (nodes.size() == 0) return;

  nodes[0].calculate_mass(this);

  for (int i = nodes.size() - 1; i >= 0; i--) {
    if (nodes[i].children_index == 0) continue;

    for (int child_index = 0; child_index < 4; child_index++) {
      QuadNode* current_child = &nodes[nodes[i].children_index + child_index];

      nodes[i].center_of_mass +=
          current_child->center_of_mass * current_child->mass;

      nodes[i].mass += current_child->mass;

      nodes[i].center_of_mass /= nodes[i].mass;
    }
  }
}

glm::vec2 QuadTree::getAcceleration(glm::vec2 position, double delta,
                                    float GRAVITATIONAL_CONSTANT,
                                    float accuracy) {
  glm::vec2 acceleration = glm::vec2(0);

  std::stack<int> index_stack;

  index_stack.push(0);

  while (!index_stack.empty()) {
    QuadNode* current_node = &nodes[index_stack.top()];
    index_stack.pop();

    float quad_distance_squared = pow(position.x - current_node->center.x, 2) +
                                  pow(position.y - current_node->center.y, 2);

    float distance_squared =
        pow(position.x - current_node->center_of_mass.x, 2) +
        pow(position.y - current_node->center_of_mass.y, 2);

    if (distance_squared < 100) continue;

    float distance = sqrt(distance_squared);

    glm::vec2 direction =
        glm::vec2((current_node->center_of_mass.x - position.x) / distance,
                  (current_node->center_of_mass.y - position.y) / distance);

    bool isAccurateEnough = current_node->size * current_node->size <
                            quad_distance_squared * accuracy;

    if (current_node->children_index == 0 || isAccurateEnough) {
      acceleration +=
          (float)(GRAVITATIONAL_CONSTANT * current_node->mass * delta) *
          direction / distance_squared;
      continue;
    }

    for (int i = 0; i < 4; i++) {
      index_stack.push(current_node->children_index + i);
    }
  }

  printf("Acceleration (%f, %f)\n", acceleration.x, acceleration.y);

  return acceleration;
}

void QuadTree::clear(std::vector<glm::vec2> included_points) {
  nodes.clear();

  if (included_points.empty()) return;

  float size;

  glm::vec2 min = glm::vec2(0), max = glm::vec2(0), center = glm::vec2(0);

  for (glm::vec2 point : included_points) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);

    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
  }

  center = (min + max) * glm::vec2(0.5);
  size = std::max(max.x - min.x, max.y - min.y);

  nodes.emplace_back(QuadNode(size, center));
}

void QuadTree::render() {
  for (int quad_index = 0; quad_index < nodes.size(); quad_index++) {
    QuadNode* quad = &nodes[quad_index];
    float line_length = quad->size;

    glm::vec2 bottom_left =
        quad->center + glm::vec2(-line_length / 2, -line_length / 2);
    glm::vec2 top_right =
        quad->center + glm::vec2(line_length / 2, line_length / 2);

    renderer->addSquare(bottom_left, top_right, line_length);
  }
}

void QuadTree::print() {
  for (int i = 0; i < nodes.size(); i++) {
    printf("[%d] | Mass: %f | Center of mass: (%f, %f) | Children: %d \n", i,
           nodes[i].mass, nodes[i].center_of_mass.x, nodes[i].center_of_mass.y,
           nodes[i].children_index);
  }
}
