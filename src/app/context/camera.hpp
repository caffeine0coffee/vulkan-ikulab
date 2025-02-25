#pragma once

#include <memory>

#include "glm/ext/matrix_double4x4.hpp"
#include "glm/ext/vector_double3.hpp"

#include "keyboard.hpp"
#include "mouse.hpp"

class Camera {
  public:
    void ResetPosition();
    void UpdateCamera(const std::shared_ptr<Mouse> &mouse_context,
                      const std::shared_ptr<Keyboard> &key_context,
                      bool is_window_focused);

    [[nodiscard]] glm::dvec3 GeneratePos() const;
    [[nodiscard]] glm::dmat4 GenerateViewMat() const;

    [[nodiscard]] auto center() const { return center_; }
    [[nodiscard]] auto h_rotation() const { return h_rotation_; }
    [[nodiscard]] auto v_rotation() const { return v_rotation_; }
    [[nodiscard]] auto distance() const { return distance_; }

    void set_center(const glm::dvec3 center) { center_ = center; }
    void set_h_rotation(const float h_rotation) { h_rotation_ = h_rotation; }
    void set_v_rotation(const float v_rotation) { v_rotation_ = v_rotation; }
    void set_distance(const float distance) { distance_ = distance; }

private:
    glm::dvec3 center_ = {};
    double h_rotation_ = 0;  /// in Radians
    double v_rotation_ = 0;  /// in Radians
    double distance_ = 0;
};
