#pragma once

#include <array>
#include <memory>

#include "../cube/singleColorCube.hpp"
#include "./debugObject.hpp"

// TODO: make another abstract class as parent

namespace ikura {
namespace shapes {
class DirectionDebugObject : public DebugObject {
    float size;
    /// x, y, z
    std::array<std::unique_ptr<SingleColorCube>, 3> axes;

  public:
    DirectionDebugObject(float size, GroupID id);
};
} // namespace shapes
} // namespace ikura