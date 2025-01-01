#include <algorithm>
#include <cmath>
#include <memory>

#include <glm/glm.hpp>

#include "../cube/singleColorCube.hpp"
#include "./stickTetrahedronBone.hpp"

namespace ikura {
namespace shapes {
StickTetrahedronBone::StickTetrahedronBone(float length, GroupID id)
    : Bone(length, id) {
    cube = std::make_unique<SingleColorCube>(length, 1.0, 1.0,
                                             glm::vec3(length / 2.0f, 0.0, 0.0),
                                             glm::vec3(0.8, 0.8, 0.8), id);
}

const std::vector<BasicVertex> &StickTetrahedronBone::getVertices() const {
    return cube->getVertices();
}

const std::vector<BasicIndex> &StickTetrahedronBone::getIndices() const {
    return cube->getIndices();
}

void StickTetrahedronBone::setBaseIndex(BasicIndex baseIndex) {
    cube->setBaseIndex(baseIndex);
}
} // namespace shapes
} // namespace ikura