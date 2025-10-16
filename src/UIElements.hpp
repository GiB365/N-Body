#ifndef NBODY_UI
#define NBODY_UI

#include <vector>
#include "Renderer.hpp"

class Button {
 private:
  std::string text;
  glm::vec2 position, size;
  glm::vec3 background_color;

  void (*clickedCallback)();

  std::vector<glm::vec2> button_corners;

  void calculateCorners();

 public:
  Button(const char* text, glm::vec2 position, glm::vec2 size,
         void (*clickedCallback)(), glm::vec3 background_color,
         glm::vec2 centered = glm::vec2(0));

  void render(Renderer* renderer);
  void checkButtonClicked(glm::vec2 mouse_position);
};

#endif
