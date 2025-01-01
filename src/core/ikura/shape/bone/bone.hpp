#pragma once

#include "../shape.hpp"

namespace ikura {
namespace shapes {

class Bone : public Shape {
    float length;

  protected:
    Bone(float length, GroupID id);
};
} // namespace shapes
} // namespace ikura