#pragma once

#include "../shape.hpp"

namespace ikura {
namespace shapes {

// just the parent class
class DebugObject : public Shape {
  protected:
    DebugObject(GroupID id);
};

} // namespace shapes
} // namespace ikura