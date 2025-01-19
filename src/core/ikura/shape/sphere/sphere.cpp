#include "sphere.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace ikura {
namespace shapes {
Sphere::Sphere(float r, uint32_t numSplitH, uint32_t numSplitV, glm::vec3 pos,
               GroupID id)
    : Shape(id), pos(pos), r(r), numSplitH(numSplitH), numSplitV(numSplitV) {}
} // namespace shapes
} // namespace ikura