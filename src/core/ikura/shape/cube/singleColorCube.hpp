#pragma once

#include <glm/glm.hpp>

#include "./gradationColorCube.hpp"

namespace ikura {
namespace shapes {
class SingleColorCube : public GradationColorCube {
    glm::vec3 color;

  public:
    SingleColorCube(float width, float height, float depth, glm::vec3 pos,
                    glm::vec3 color, GroupID id);
};
} // namespace shapes
} // namespace ikura