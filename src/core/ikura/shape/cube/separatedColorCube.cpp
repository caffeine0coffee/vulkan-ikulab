#include "./separatedColorCube.hpp"

namespace ikura {
namespace shapes {
SeparatedColorCube::SeparatedColorCube(float width, float height, float depth,
                                       glm::vec3 pos,
                                       std::array<glm::vec3, 6> colors,
                                       GroupID id)
    : Cube(width, height, depth, pos, id), colors(colors) {

    this->id = id;
    glm::vec3 leftDownBottom{-width / 2.0f + pos.x, -height / 2.0f + pos.y,
                             -depth / 2.0f + pos.z};
    glm::vec3 leftDownTop = {-width / 2.0f + pos.x, -height / 2.0f + pos.y,
                             depth / 2.0f + pos.z};
    glm::vec3 leftUpBottom = {-width / 2.0f + pos.x, height / 2.0f + pos.y,
                              -depth / 2.0f + pos.z};
    glm::vec3 leftUpTop = {-width / 2.0f + pos.x, height / 2.0f + pos.y,
                           depth / 2.0f + pos.z};
    glm::vec3 rightDownBottom = {width / 2.0f + pos.x, -height / 2.0f + pos.y,
                                 -depth / 2.0f + pos.z};
    glm::vec3 rightDownTop = {width / 2.0f + pos.x, -height / 2.0f + pos.y,
                              depth / 2.0f + pos.z};
    glm::vec3 rightUpBottom = {width / 2.0f + pos.x, height / 2.0f + pos.y,
                               -depth / 2.0f + pos.z};
    glm::vec3 rightUpTop = {width / 2.0f + pos.x, height / 2.0f + pos.y,
                            depth / 2.0f + pos.z};

    vertices.assign({// 0-3 Top face - red
                     {leftUpTop, colors[0], id},
                     {leftDownTop, colors[0], id},
                     {rightDownTop, colors[0], id},
                     {rightUpTop, colors[0], id},
                     // 4-7 Bottom face - green
                     {leftUpBottom, colors[1], id},
                     {rightUpBottom, colors[1], id},
                     {rightDownBottom, colors[1], id},
                     {leftDownBottom, colors[1], id},
                     // 8-11 Left face - blue
                     {leftUpTop, colors[2], id},
                     {leftUpBottom, colors[2], id},
                     {leftDownBottom, colors[2], id},
                     {leftDownTop, colors[2], id},
                     // 12-15 Right face - purple
                     {rightUpTop, colors[3], id},
                     {rightDownTop, colors[3], id},
                     {rightDownBottom, colors[3], id},
                     {rightUpBottom, colors[3], id},
                     // 16-19 Front face - cyan
                     {leftDownTop, colors[4], id},
                     {leftDownBottom, colors[4], id},
                     {rightDownBottom, colors[4], id},
                     {rightDownTop, colors[4], id},
                     // 20-23 Back face - yellow
                     {rightUpTop, colors[5], id},
                     {rightUpBottom, colors[5], id},
                     {leftUpBottom, colors[5], id},
                     {leftUpTop, colors[5], id}});

    indices.assign({// Top
                    0, 1, 2, 2, 3, 0,
                    // Bottom
                    4, 5, 6, 6, 7, 4,
                    // Left but Top
                    8, 9, 10, 10, 11, 8,
                    // Right but Bottom
                    12, 13, 14, 14, 15, 12,
                    // Front but Left
                    16, 17, 18, 18, 19, 16,
                    // Back but Right
                    20, 21, 22, 22, 23, 20});
}
} // namespace shapes
} // namespace ikura