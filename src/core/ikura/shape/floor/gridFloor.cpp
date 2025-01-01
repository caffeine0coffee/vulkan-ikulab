#include "./gridFloor.hpp"

namespace ikura {
namespace shapes {
GridFloor::GridFloor(float width, float height, float lineWidth, int numSplitX,
                     int numSplitY, glm::vec3 color, GroupID id)
    : Floor(width, height, id), lineWidth(lineWidth), numSplitX(numSplitX),
      numSPlitY(numSplitY) {

    const float xInterval = width / numSplitY;
    const float yInterval = height / numSplitX;

    uint32_t idx = 0;
    // Y-axis lines
    for (uint32_t x = 0; x < numSplitX + 1; x++) {
        float currentX = x * xInterval - width / 2;
        vertices.insert(
            vertices.end(),
            {{{currentX - lineWidth / 2, height / 2 + lineWidth / 2, 0.0},
              color,
              id},
             {{currentX - lineWidth / 2, -height / 2 - lineWidth / 2, 0.0},
              color,
              id},
             {{currentX + lineWidth / 2, -height / 2 - lineWidth / 2, 0.0},
              color,
              id},
             {{currentX + lineWidth / 2, height / 2 + lineWidth / 2, 0.0},
              color,
              id}});
        indices.insert(indices.end(),
                       {/* front side */
                        idx, idx + 1, idx + 2, idx + 2, idx + 3, idx,
                        /* back side */
                        idx, idx + 3, idx + 2, idx + 2, idx + 1, idx});
        idx += 4;

        // X-axis lines
        if (x == numSplitX) {
            break;
        }
        for (uint32_t y = 0; y < numSplitY + 1; y++) {
            float nextX = currentX + xInterval;
            float currentY = y * yInterval - height / 2;
            vertices.insert(
                vertices.end(),
                {{{currentX + lineWidth / 2, currentY - lineWidth / 2, 0.0},
                  color,
                  id},
                 {{currentX + lineWidth / 2, currentY + lineWidth / 2, 0.0},
                  color,
                  id},
                 {{nextX - lineWidth / 2, currentY + lineWidth / 2, 0.0},
                  color,
                  id},
                 {{nextX - lineWidth / 2, currentY - lineWidth / 2, 0.0},
                  color,
                  id}});
            indices.insert(indices.end(),
                           {/* front side */
                            idx, idx + 1, idx + 2, idx + 2, idx + 3, idx,
                            /* back side */
                            idx, idx + 3, idx + 2, idx + 2, idx + 1, idx});
            idx += 4;
        }
    }
}
} // namespace shapes
} // namespace ikura