#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../cube/singleColorCube.hpp"
#include "./bone.hpp"

namespace ikura {
namespace shapes {
class StickTetrahedronBone : public Bone {
    std::unique_ptr<SingleColorCube> cube;

  public:
    StickTetrahedronBone(float length, GroupID id);

    const std::vector<BasicVertex> &getVertices() const override;
    const std::vector<uint32_t> &getIndices() const override;

    void setBaseIndex(BasicIndex baseIndex) override;
};
} // namespace shapes
} // namespace ikura