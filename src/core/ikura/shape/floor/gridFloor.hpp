#pragma once

#include "./floor.hpp"

namespace ikura {
namespace shapes {
class GridFloor : public Floor {
    glm::vec3 color;
    float lineWidth;
    int numSplitX, numSPlitY;

  public:
    GridFloor(float width, float height, float lineWidth, int numSplitX,
              int numSplitY, glm::vec3 color, GroupID id);
};
} // namespace shapes
} // namespace ikura