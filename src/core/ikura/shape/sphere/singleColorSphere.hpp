#pragma once

#include "./sphere.hpp"

namespace ikura {
namespace shapes {
class SingleColorSphere : public Sphere {
  protected:
    glm::vec3 color;

  public:
    SingleColorSphere(float r, uint32_t numSplitH, uint32_t numSplitV,
                      glm::vec3 pos, glm::vec3 color, GroupID id);
};
} // namespace shapes
} // namespace ikura