#pragma once

#include <glm/glm.hpp>

#include "../shape.hpp"

namespace ikura {
namespace shapes {
class Sphere : public Shape {
  protected:
    glm::vec3 pos;
    float r;
    uint32_t numSplitH, numSplitV;

  public:
    Sphere(float r, uint32_t numSplitH, uint32_t numSplitV, glm::vec3 pos,
           GroupID id);
};
} // namespace shapes
} // namespace ikura