#include <algorithm>

#include "./shape.hpp"

namespace ikura {
namespace shapes {
Shape::Shape(GroupID id) : id(id) {}

const std::vector<BasicVertex> &Shape::getVertices() const { return vertices; };

const std::vector<BasicIndex> &Shape::getIndices() const { return indices; };

void Shape::setBaseIndex(BasicIndex baseIndex) {
    this->baseIndex = baseIndex;

    std::for_each(indices.begin(), indices.end(),
                  [=](BasicIndex &index) { index += baseIndex; });
}

BasicIndex Shape::getBaseIndex() { return baseIndex; }
} // namespace shapes
} // namespace ikura