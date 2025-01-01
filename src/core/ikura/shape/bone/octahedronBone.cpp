#include "./octahedronBone.hpp"
#include "../sphere/singleColorSphere.hpp"

namespace ikura {
namespace shapes {
OctahedronBone::OctahedronBone(float length, GroupID id) : Bone(length, id) {
    glm::vec3 root(length, 0.0, 0.0);
    glm::vec3 tip(0, 0.0, 0.0);

    glm::vec3 midTop(length * 0.9, 0.0, length * 0.1);
    glm::vec3 midBottom(length * 0.9, 0.0, -length * 0.1);
    glm::vec3 midLeft(length * 0.9, -length * 0.1, 0.0);
    glm::vec3 midRight(length * 0.9, length * 0.1, 0.0);

    glm::vec3 rootColorGray(0.3, 0.3, 0.3);  // top-right and bottom-left area
    glm::vec3 rootColorWhite(0.7, 0.7, 0.7); // top-left and bottom-right area
    glm::vec3 tipColorGray(0.5, 0.5, 0.5);   // top-right and bottom-left area
    glm::vec3 tipColorWhite(0.9, 0.9, 0.9);  // top-left and bottom-right area

    // root side ---
    // top-right area
    vertices.insert(vertices.end(), {{root, rootColorGray, id},
                                     {midRight, rootColorGray, id},
                                     {midTop, rootColorGray, id}});
    // bottom-right area
    vertices.insert(vertices.end(), {
                                        {root, rootColorWhite, id},
                                        {midBottom, rootColorWhite, id},
                                        {midRight, rootColorWhite, id},
                                    });

    // bottom-left area
    vertices.insert(vertices.end(), {{root, rootColorGray, id},
                                     {midLeft, rootColorGray, id},
                                     {midBottom, rootColorGray, id}});
    // top-left area
    vertices.insert(vertices.end(), {{root, rootColorWhite, id},
                                     {midTop, rootColorWhite, id},
                                     {midLeft, rootColorWhite, id}});

    // tip side ---
    vertices.insert(vertices.end(), {{tip, tipColorGray, id},
                                     {midTop, tipColorGray, id},
                                     {midRight, tipColorGray, id}});
    // bottom-right area
    vertices.insert(vertices.end(), {{tip, tipColorWhite, id},
                                     {midRight, tipColorWhite, id},
                                     {midBottom, tipColorWhite, id}});
    // bottom-left area
    vertices.insert(vertices.end(), {{tip, tipColorGray, id},
                                     {midBottom, tipColorGray, id},
                                     {midLeft, tipColorGray, id}});
    // top-left area
    vertices.insert(vertices.end(), {{tip, tipColorWhite, id},
                                     {midLeft, tipColorWhite, id},
                                     {midTop, tipColorWhite, id}});

    // add indices
    for (int i = 0; i < vertices.size(); i++) {
        indices.push_back(i);
    }

    // root and tip Spheres ---
    SingleColorSphere rootSphere(length * 0.03, 10, 10,
                                 glm::vec3(length, 0.0, 0.0),
                                 glm::vec3(0.8, 0.8, 0.8), id);
    SingleColorSphere tipSphere(length * 0.02, 10, 10, glm::vec3(0.0, 0.0, 0.0),
                                glm::vec3(0.8, 0.8, 0.8), id);

    BasicIndex baseIndex = static_cast<uint32_t>(vertices.size());
    rootSphere.setBaseIndex(baseIndex);
    baseIndex += static_cast<uint32_t>(rootSphere.getVertices().size());
    tipSphere.setBaseIndex(baseIndex);

    vertices.insert(vertices.end(), rootSphere.getVertices().begin(),
                    rootSphere.getVertices().end());
    vertices.insert(vertices.end(), tipSphere.getVertices().begin(),
                    tipSphere.getVertices().end());
    indices.insert(indices.end(), rootSphere.getIndices().begin(),
                   rootSphere.getIndices().end());
    indices.insert(indices.end(), tipSphere.getIndices().begin(),
                   tipSphere.getIndices().end());
}
} // namespace shapes
} // namespace ikura