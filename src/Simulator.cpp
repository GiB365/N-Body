#include "Simulator.hpp"
#include <cmath>
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <vector>
#include "Renderer.hpp"

const float GRAVITATIONAL_CONSTANT = 10000;

int rand_range(int min, int max);

Body::Body(std::string name, float mass, int radius, glm::vec2 position,
           glm::vec3 color, glm::vec2 velocity, int max_trail_length) {
  this->name = name;
  this->mass = mass;
  this->radius = radius;
  this->position = position;
  this->color = color;
  this->velocity = velocity;
  this->max_trail_length = max_trail_length;
}

void Body::renderTrail(Renderer* renderer) {
  for (int i = 0; i < trail_points.size(); i++) {
    renderer->addCircle(trail_points[i], radius * 0.3, color * glm::vec3(0.8));
  }
}

void Body::renderBody(Renderer* renderer) {
  renderer->addCircle(position, radius, color);
}

void Body::renderName(Renderer* renderer) {
  if (name.empty()) {
    return;
  }

  renderer->addText(name, position, 0.5, false);
}

Simulator::Simulator(Renderer* renderer, int body_count) {
  this->renderer = renderer;
  this->tree = new QuadTree(renderer);
  perspective = -1;

  for (int i = 0; i < body_count; i++) {
    float body_mass = rand_range(10, 25);
    float x = rand_range(-1000, 1000);
    float y = rand_range(-500, 500);

    addBody(body_mass, body_mass, glm::vec2(x, y));
  }
}

void Simulator::update(SimulationMethod method, float delta, bool show_names,
                       bool show_trails) {
  switch (method) {
    case SimulationMethod::Simple:
      simple(delta);
      break;

    case SimulationMethod::BarnesHut:
      barnesHut(delta);
      break;

    case SimulationMethod::FastMultipole:
      fastMultipole(delta);
      break;
  }

  for (int i = 0; i < bodies.size(); i++) {
    if (perspective != -1) {
      bodies[i].position -= bodies[perspective].velocity * delta;
    }

    bodies[i].position += bodies[i].velocity * delta;

    if (i == perspective) {
      continue;
    }

    float max_trail_distance = bodies[i].radius;
    float trail_distance = max_trail_distance;

    if (!bodies[i].trail_points.empty()) {
      glm::vec2 last_trail_point = bodies[i].trail_points.back();

      trail_distance = sqrt(pow(last_trail_point.x - bodies[i].position.x, 2) +
                            pow(last_trail_point.y - bodies[i].position.y, 2));
    }

    if (trail_distance >= max_trail_distance) {
      bodies[i].trail_points.emplace_back(bodies[i].position);
    }

    if (bodies[i].trail_points.size() >=
        100 * bodies[i].max_trail_length / bodies[i].radius) {
      bodies[i].trail_points.erase(bodies[i].trail_points.begin());
    }
  }

  render(show_names, show_trails);
}

void Simulator::simple(double delta) {
  for (int affected_index = 0; affected_index < bodies.size();
       affected_index++) {
    for (int affecting_index = 0; affecting_index < bodies.size();
         affecting_index++) {
      if (affected_index == affecting_index) continue;

      glm::vec2 affected_position = bodies[affected_index].position;

      glm::vec2 affecting_position = bodies[affecting_index].position;
      float affecting_mass = bodies[affecting_index].mass;

      float distance_squared =
          pow(affecting_position.x - affected_position.x, 2) +
          pow(affecting_position.y - affected_position.y, 2);

      if (distance_squared < 100) continue;

      float distance = sqrt(distance_squared);

      glm::vec2 direction =
          glm::vec2((affecting_position.x - affected_position.x) / distance,
                    (affecting_position.y - affected_position.y) / distance);

      if (distance_squared < 1) {
        continue;
      }

      bodies[affected_index].velocity +=
          (float)(GRAVITATIONAL_CONSTANT * affecting_mass * delta) * direction /
          distance_squared;
    }
  }
}

void Simulator::barnesHut(double delta) {
  std::vector<glm::vec2> positions;

  for (Body body : bodies) {
    positions.emplace_back(body.position);
  }

  tree->clear(positions);

  for (Body body : bodies) {
    tree->addPoint(body.position, body.mass);
  }

  tree->calculate_masses();

  for (int i = 0; i < bodies.size(); i++) {
    bodies[i].velocity += tree->getAcceleration(bodies[i].position, delta,
                                                GRAVITATIONAL_CONSTANT, 1.0);
    std::cout << i << std::endl;
  }

  tree->print();

  tree->render();
}

void Simulator::fastMultipole(double delta) {
  std::cerr << "Unimplemented" << std::endl;
}

void Simulator::render(bool show_names, bool show_trails) {
  if (show_names) {
    for (int i = 0; i < bodies.size(); i++) {
      bodies[i].renderName(renderer);
    }
  }

  if (show_trails) {
    for (int i = 0; i < bodies.size(); i++) {
      bodies[i].renderTrail(renderer);
    }
  }

  for (int i = 0; i < bodies.size(); i++) {
    bodies[i].renderBody(renderer);
  }
}

void Simulator::addBody(float mass, int radius, glm::vec2 position,
                        glm::vec3 color, glm::vec2 velocity, std::string name,
                        int max_trail_length) {
  bodies.emplace_back(
      Body(name, mass, radius, position, color, velocity, max_trail_length));
}

void Simulator::clearBodies() { bodies.clear(); }

int Simulator::getBodyCount() { return bodies.size(); }

void Simulator::changePerspective(Renderer* renderer, int new_perspective,
                                  bool change_positions) {
  perspective = new_perspective;

  if (bodies.empty()) return;

  if (change_positions) {
    glm::vec2 perspective_position = bodies[perspective].position;

    for (int i = 0; i < bodies.size(); i++) {
      bodies[i].position -= perspective_position;
    }

    renderer->camera_position = glm::vec2(0);
  }

  for (int i = 0; i < bodies.size(); i++) {
    bodies[i].trail_points.clear();
  }
}

std::string Simulator::getPerspectiveName() { return bodies[perspective].name; }

int rand_range(int min, int max) { return min + rand() % (max - min + 1); }
