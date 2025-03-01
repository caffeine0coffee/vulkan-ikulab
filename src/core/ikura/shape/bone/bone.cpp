#include "bone.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <vector>

#include "shape/sphere/sphere.hpp"

namespace ikura {
namespace shapes {
Bone::Bone(float length, GroupID id) : Shape(id), length(length) {}
} // namespace shapes
} // namespace ikura