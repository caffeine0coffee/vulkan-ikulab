#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "../shape.hpp"

namespace ikura {
namespace shapes {
class Cube : public Shape {
  protected:
    float width, height, depth;
    glm::vec3 pos;

  public:
    Cube(float width, float height, float depth, glm::vec3 pos, GroupID id);
};
} // namespace shapes
} // namespace ikura