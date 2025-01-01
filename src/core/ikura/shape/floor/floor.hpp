#pragma once

#include "../shape.hpp"

namespace ikura {
namespace shapes {
class Floor : public Shape {
    float width, height;

  public:
    Floor(float width, float height, GroupID id);
};
} // namespace shapes
} // namespace ikura