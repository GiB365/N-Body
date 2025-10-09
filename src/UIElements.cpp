#include "UIElements.hpp"
#include <iostream>

Button::Button(const char* text, glm::vec2 position, glm::vec2 size,
               void (*clickedCallback)(), glm::vec3 background_color) {
  this->text = text;
  this->x = position.x;
  this->y = position.y;
  this->width = size.x;
  this->height = size.y;
  this->clickedCallback = clickedCallback;
  this->background_color = background_color;

  this->calculateCorners();
}

void Button::checkButtonClicked(glm::vec2 mouse_screen_position) {
  float x_distance = 100 * mouse_screen_position.x - x;
  float y_distance = 100 * mouse_screen_position.y - y;

  bool within_x = x_distance > 0 && x_distance < width;
  bool within_y = y_distance > 0 && y_distance < height;

  std::cout << mouse_screen_position.x << " " << mouse_screen_position.y
            << std::endl;

  std::cout << x_distance << " " << y_distance << std::endl;

  std::cout << width << " " << height << std::endl;

  if (!within_x || !within_y) {
    return;
  }

  clickedCallback();
}

void Button::render(Renderer* renderer) {
  renderer->addTriangle(button_corners[0], button_corners[1], button_corners[2],
                        background_color);
  renderer->addTriangle(button_corners[1], button_corners[2], button_corners[3],
                        background_color);
}

void Button::calculateCorners() {
  button_corners.emplace_back(glm::vec2(x, -y));
  button_corners.emplace_back(glm::vec2(x + width, -y));
  button_corners.emplace_back(glm::vec2(x, -y - height));
  button_corners.emplace_back(glm::vec2(x + width, -y - height));
}
