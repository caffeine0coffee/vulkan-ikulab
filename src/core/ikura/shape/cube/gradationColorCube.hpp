#pragma once

#include <array>
#include <glm/glm.hpp>

#include "./cube.hpp"

// TODO: init()をなくす
// SingleColorCube() からは、全て同じ要素のvec3[]をGradationCube()に渡す

namespace ikura {
namespace shapes {
class GradationColorCube : public Cube {
  protected:
    /**
     * Top face
     *  0: left top
     *  1: right top
     *  2: right bottom
     *  3: left bottom
     * Bottom face
     *  4: left top
     *  5: right top
     *  6: right bottom
     *  7: left bottom
     */
    std::array<glm::vec3, 8> colors;

  public:
    GradationColorCube(float width, float height, float depth, glm::vec3 pos,
                       std::array<glm::vec3, 8> colors, GroupID id);

    void init();
};
} // namespace shapes
} // namespace ikura