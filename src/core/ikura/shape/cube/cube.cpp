#include "cube.hpp"

#include <algorithm>
#include <iostream>

namespace ikura {
namespace shapes {
Cube::Cube(float width, float height, float depth, glm::vec3 pos, GroupID id)
    : Shape(id), width(width), height(height), depth(depth), pos(pos) {}
} // namespace shapes
} // namespace ikura