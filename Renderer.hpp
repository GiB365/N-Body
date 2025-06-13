#include <utility>
#include <vector>

class Renderer {
 private:
  std::vector<std::pair<float, int>> shapes;

 public:
  Renderer();

  void addShape(float radius, int sides);

  std::vector<float> genVertices();

  void render(bool clear = true);
};
