#pragma once

#include "./floor.hpp"

namespace ikura {
namespace shapes {
class FilledFloor : public Floor {
    glm::vec3 color;

  public:
    FilledFloor(float width, float height, glm::vec3 color, bool frontSideOnly,
                GroupID id);
};
} // namespace shapes
} // namespace ikura