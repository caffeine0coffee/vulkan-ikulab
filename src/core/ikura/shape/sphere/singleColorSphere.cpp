#include "./singleColorSphere.hpp"

#include <cmath>

namespace ikura {
namespace shapes {

SingleColorSphere::SingleColorSphere(float r, uint32_t numSplitH,
                                     uint32_t numSplitV, glm::vec3 pos,
                                     glm::vec3 color, GroupID id)
    : Sphere(r, numSplitH, numSplitV, pos, id) {

    // Top vertex (n = 0)
    vertices.push_back({{pos.x, pos.y, pos.z + r}, color, id});

    for (uint32_t n = 1; n < numSplitH; n++) {
        for (uint32_t m = 0; m < numSplitV; m++) {
            float degXY = ((M_PI * 2) / numSplitV) * m;
            float degZ = M_PI / 2 - (M_PI / numSplitH) * n;

            float x = r * std::cos(degXY) * std::cos(degZ);
            float y = r * std::sin(degXY) * std::cos(degZ);
            float z = r * std::sin(degZ);

            vertices.push_back({{pos.x + x, pos.y + y, pos.z + z}, color, id});
        }
    }

    // Bottom vertex (n = numSplitH)
    vertices.push_back({{pos.x, pos.y, pos.z - r}, color, id});

    // Top index (n = 1)
    for (uint32_t m = 0; m < numSplitV; m++) {
        indices.insert(indices.end(), {0, m + 1, (m + 1) % numSplitV + 1});
    }

    for (uint32_t n = 2; n < numSplitH; n++) {
        for (uint32_t m = 0; m < numSplitV; m++) {
            indices.insert(indices.end(),
                           {numSplitV * (n - 1) + m + 1,
                            numSplitV * (n - 1) + (m + 1) % numSplitV + 1,
                            numSplitV * (n - 2) + m + 1});
            indices.insert(indices.end(),
                           {numSplitV * (n - 1) + (m + 1) % numSplitV + 1,
                            numSplitV * (n - 2) + (m + 1) % numSplitV + 1,
                            numSplitV * (n - 2) + m + 1});
        }
    }

    // Bottom index (n = numSplitH)
    for (uint32_t m = 0; m < numSplitV; m++) {
        indices.insert(indices.end(),
                       {(numSplitH - 1) * numSplitV + 1,
                        (numSplitH - 2) * numSplitV + (m + 1) % numSplitV + 1,
                        (numSplitH - 2) * numSplitV + m + 1});
    }
}

} // namespace shapes
} // namespace ikura