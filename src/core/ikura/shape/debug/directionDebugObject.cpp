#include <vector>

#include "./directionDebugObject.hpp"

namespace ikura {
namespace shapes {
DirectionDebugObject::DirectionDebugObject(float size, GroupID id)
    : DebugObject(id), size(size) {
    float thickness = size / 8;
    axes[0] = std::make_unique<SingleColorCube>(
        size, thickness, thickness,
        glm::vec3(size / 2 + thickness / 2, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0),
        id);
    axes[1] = std::make_unique<SingleColorCube>(
        thickness, size, thickness,
        glm::vec3(0.0, size / 2 + thickness / 2, 0.0), glm::vec3(0.0, 1.0, 0.0),
        id);
    axes[2] = std::make_unique<SingleColorCube>(
        thickness, thickness, size,
        glm::vec3(0.0, 0.0, size / 2 + thickness / 2), glm::vec3(0.0, 0.0, 1.0),
        id);

    // vertices
    for (const auto &ax : axes) {
        const std::vector<BasicVertex> &vec = ax->getVertices();
        vertices.insert(vertices.end(), vec.begin(), vec.end());
    }

    // indices
    BasicIndex baseIndex = 0;
    for (const auto &ax : axes) {
        ax->setBaseIndex(baseIndex);
        const std::vector<BasicIndex> &idx = ax->getIndices();
        indices.insert(indices.end(), idx.begin(), idx.end());

        baseIndex += ax->getVertices().size();
    }
}
} // namespace shapes
} // namespace ikura