#include "camera.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

#include "glm/ext/matrix_double4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_double3.hpp"
#include "glm/ext/vector_double4.hpp"
#include "glm/trigonometric.hpp"

#include "keyboard.hpp"
#include "mouse.hpp"

namespace {
constexpr glm::dvec3 kDefaultCenter = {0.0, 0.0, 5.0};
constexpr double kDefaultHRotation = 0.0;
constexpr double kDefaultVRotation = glm::radians(20.0);
constexpr double kDefaultDistance = 50.0;
}  // namespace

void Camera::ResetPosition() {
    center_ = kDefaultCenter;
    h_rotation_ = kDefaultHRotation;
    v_rotation_ = kDefaultVRotation;
    distance_ = kDefaultDistance;
}

void Camera::UpdateCamera(const std::shared_ptr<Mouse> &mouseCtx,
                          const std::shared_ptr<Keyboard> &keyCtx,
                          const bool isWindowFocused) {
    static constexpr float ROTATION_DIFF_RATIO = 0.01;
    static constexpr float SHIFT_DIFF_RATIO = 0.1;
    static constexpr float SCROLL_RATIO = 1.1;

    if (!isWindowFocused) {
        if (mouseCtx->leftButton) {
            double xDiff = mouseCtx->deltaX;
            double yDiff = mouseCtx->deltaY;

            if (keyCtx->shift) {
                xDiff *= SHIFT_DIFF_RATIO;
                yDiff *= SHIFT_DIFF_RATIO;

                glm::dmat4 rotate_mat(1.0);
                rotate_mat *= glm::rotate(glm::dmat4(1.0), h_rotation_,
                                          glm::dvec3(0.0, 0.0, 1.0));
                rotate_mat *= glm::rotate(glm::dmat4(1.0), -v_rotation_,
                                          glm::dvec3(0.0, 1.0, 0.0));

                const glm::dvec4 move_mat(0.0, -xDiff, yDiff, 1.0);

                center_ += glm::dvec3(rotate_mat * move_mat);
            } else {
                xDiff *= ROTATION_DIFF_RATIO;
                yDiff *= ROTATION_DIFF_RATIO;

                h_rotation_ = std::fmod(h_rotation_ - xDiff, 2 * M_PI);
                v_rotation_ =
                    std::clamp(std::fmod(v_rotation_ + yDiff, 2 * M_PI),
                               -M_PI / 2.0 + 0.0001, M_PI / 2.0 - 0.0001);
            }
        }
        distance_ *= std::pow(SCROLL_RATIO, -mouseCtx->scrollOffsetY);
    }
}

glm::dvec3 Camera::GeneratePos() const {
    glm::dvec3 pos;
    pos.x = distance_ * std::cos(h_rotation_) * std::cos(v_rotation_);
    pos.y = distance_ * std::sin(h_rotation_) * std::cos(v_rotation_);
    pos.z = distance_ * std::sin(v_rotation_);
    pos += center_;
    return pos;
}

glm::dmat4 Camera::GenerateViewMat() const {
    return glm::lookAt(GeneratePos(), center_, glm::dvec3(0.0, 0.0, 1.0));
}
