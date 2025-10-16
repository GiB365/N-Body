#include "UIElements.hpp"
#include "glm/ext/vector_float2.hpp"

Button::Button(const char* text, glm::vec2 position, glm::vec2 size,
               void (*clickedCallback)(), glm::vec3 background_color,
               glm::vec2 centered) {
  this->text = text;
  this->position = position;
  this->size = size;
  this->clickedCallback = clickedCallback;
  this->background_color = background_color;

  if (centered.x) {
    this->position.x = position.x - size.x / 2.0;
  }
  if (centered.y) {
    this->position.y = position.y - size.y / 2.0;
  }

  this->calculateCorners();
}

void Button::checkButtonClicked(glm::vec2 mouse_screen_position) {
  if (!visible) {
    return;
  }

  float x_distance = abs(100 * mouse_screen_position.x - position.x);
  float y_distance = abs(100 * mouse_screen_position.y - position.y);

  bool within_x = x_distance > 0 && x_distance < size.x;
  bool within_y = y_distance > 0 && y_distance < abs(size.y);

  if (!within_x || !within_y || clickedCallback == nullptr) {
    return;
  }

  clickedCallback();
}

void Button::render(Renderer* renderer) {
  if (!visible) {
    return;
  }

  if (background_color != glm::vec3(0.0)) {
    renderer->addTriangle(button_corners[0], button_corners[1],
                          button_corners[2], background_color);
    renderer->addTriangle(button_corners[1], button_corners[2],
                          button_corners[3], background_color);
  }

  glm::vec2 text_position = glm::vec2((2 * position.x + size.x) / 2.0,
                                      (2 * position.y + size.y) / 2.0);

  renderer->addText(text, text_position, size.y, true);
}

void Button::calculateCorners() {
  button_corners.emplace_back(glm::vec2(position.x, position.y));
  button_corners.emplace_back(glm::vec2(position.x + size.x, position.y));
  button_corners.emplace_back(glm::vec2(position.x, position.y + size.y));
  button_corners.emplace_back(
      glm::vec2(position.x + size.x, position.y + size.y));
}
