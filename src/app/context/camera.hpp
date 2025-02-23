#pragma once

#include <memory>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

#include "keyboard.hpp"
#include "mouse.hpp"

class Camera {
  public:
    void Init();
    void UpdateCamera(std::shared_ptr<Mouse> mouseCtx,
                      std::shared_ptr<Keyboard> keyCtx, bool isWindowFocused);

    glm::vec3 GeneratePos();
    glm::mat4 GenerateViewMat();

    [[nodiscard]] auto center() const { return center_; }
    [[nodiscard]] auto h_rotation() const { return h_rotation_; }
    [[nodiscard]] auto v_rotation() const { return v_rotation_; }
    [[nodiscard]] auto distance() const { return distance_; }

    void set_center(const glm::vec3 center) { center_ = center; }
    void set_h_rotation(const float h_rotation) { h_rotation_ = h_rotation; }
    void set_v_rotation(const float v_rotation) { v_rotation_ = v_rotation; }
    void set_distance(const float distance) { distance_ = distance; }

private:
    glm::vec3 center_;
    /// in Radians
    float h_rotation_;
    /// in Radians
    float v_rotation_;
    float distance_;
};
