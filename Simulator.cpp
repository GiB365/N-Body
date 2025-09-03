#include "Simulator.hpp"
#include <cmath>
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>
#include <iostream>

const float GRAVITATIONAL_CONSTANT = 10000;

int rand_range(int min, int max);

Body::Body(float mass, int radius, glm::vec2 position, glm::vec3 color,
           glm::vec2 velocity) {
  this->mass = mass;
  this->radius = radius;
  this->position = position;
  this->color = color;
  this->velocity = velocity;
}

void Body::render(Renderer* renderer) {
  renderer->addCircle(radius, position, color);
}

Simulator::Simulator(Renderer* renderer, int body_count) {
  this->renderer = renderer;
  this->tree = new QuadTree(renderer);

  for (int i = 0; i < body_count; i++) {
    float body_mass = rand_range(10, 25);
    float x = rand_range(-1000, 1000);
    float y = rand_range(-500, 500);

    addBody(body_mass, body_mass, glm::vec2(x, y));
  }
}
void Simulator::update(SimulationMethod method, double delta) {
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
    bodies[i].position += bodies[i].velocity * (float)delta;
  }

  render();
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

void Simulator::render() {
  for (int i = 0; i < bodies.size(); i++) {
    bodies[i].render(renderer);
  }
}

void Simulator::addBody(float mass, int radius, glm::vec2 position,
                        glm::vec3 color, glm::vec2 velocity) {
  bodies.emplace_back(Body(mass, radius, position, color, velocity));
}

int rand_range(int min, int max) { return min + rand() % (max - min + 1); }
