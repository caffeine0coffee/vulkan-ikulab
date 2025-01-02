#include <algorithm>
#include <iostream>

#include "cube.hpp"

namespace ikura {
namespace shapes {
Cube::Cube(float width, float height, float depth, glm::vec3 pos, GroupID id)
    : Shape(id), width(width), height(height), depth(depth), pos(pos) {}
} // namespace shapes
} // namespace ikura