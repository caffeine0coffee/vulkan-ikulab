#pragma once

#include <iostream>
#include <vector>

#include "../common/renderPrimitiveTypes.hpp"

namespace ikura {
namespace shapes {
typedef uint32_t GroupID;

class Shape {
  protected:
    std::vector<BasicVertex> vertices;
    std::vector<BasicIndex> indices;
    GroupID id;
    BasicIndex baseIndex = 0;

  public:
    Shape(GroupID id);

    virtual const std::vector<BasicVertex> &getVertices() const;
    virtual const std::vector<BasicIndex> &getIndices() const;

    virtual void setBaseIndex(BasicIndex baseIndex);
    virtual BasicIndex getBaseIndex();
};
} // namespace shapes
} // namespace ikura