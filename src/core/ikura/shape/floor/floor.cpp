#include "./floor.hpp"

namespace ikura {
namespace shapes {
Floor::Floor(float width, float height, GroupID id)
    : width(width), height(height), Shape(id) {}
} // namespace shapes
} // namespace ikura