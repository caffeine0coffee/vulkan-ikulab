#include "./gradationColorCube.hpp"

namespace ikura {
namespace shapes {
GradationColorCube::GradationColorCube(float width, float height, float depth,
                                       glm::vec3 pos,
                                       std::array<glm::vec3, 8> colors,
                                       GroupID id)
    : Cube(width, height, depth, pos, id), colors(colors) {
    init();
}

void GradationColorCube::init() {
    float xp = width / 2.0f + pos.r;
    float xn = -width / 2.0f + pos.r;
    float yp = height / 2.0f + pos.g;
    float yn = -height / 2.0f + pos.g;
    float zp = depth / 2.0 + pos.b;
    float zn = -depth / 2.0 + pos.b;

    vertices.assign({// Top face (Z = 0.5)
                     // 0: left top
                     {{xn, yn, zp}, {colors[0]}, id},
                     // 1: right top
                     {{xp, yn, zp}, {colors[1]}, id},
                     // 2: right bottom
                     {{xp, yp, zp}, {colors[2]}, id},
                     // 3: left bottom
                     {{xn, yp, zp}, {colors[3]}, id},
                     // Bottom face (Z = -0.5)
                     // 4: left top
                     {{xn, yn, zn}, {colors[4]}, id},
                     // 5: right top
                     {{xp, yn, zn}, {colors[5]}, id},
                     // 6: right bottom
                     {{xp, yp, zn}, {colors[6]}, id},
                     // 7: left bottom
                     {{xn, yp, zn}, {colors[7]}, id}});

    indices.assign({// Top
                    0, 1, 2, 2, 3, 0,
                    // Bottom
                    4, 7, 6, 6, 5, 4,
                    // Left
                    0, 3, 7, 7, 4, 0,
                    // Right
                    1, 5, 6, 6, 2, 1,
                    // Front
                    2, 6, 7, 7, 3, 2,
                    // Back
                    0, 4, 5, 5, 1, 0});
}
} // namespace shapes
} // namespace ikura