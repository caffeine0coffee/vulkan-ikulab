#include <cmath>
#include <glm/glm.hpp>
#include <vector>

#include "../sphere/sphere.hpp"
#include "./bone.hpp"

namespace ikura {
namespace shapes {
Bone::Bone(float length, GroupID id) : Shape(id), length(length) {}
} // namespace shapes
} // namespace ikura