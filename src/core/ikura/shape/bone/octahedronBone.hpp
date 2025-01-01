#pragma once

#include "./bone.hpp"

namespace ikura {
namespace shapes {
class OctahedronBone : public Bone {
  public:
    OctahedronBone(float length, GroupID id);
};
} // namespace shapes
} // namespace ikura