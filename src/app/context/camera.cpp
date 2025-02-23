#include "camera.hpp"

#include <algorithm>

#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"

void Camera::Init() {
    center_ = {0.0, 0.0, 5.0};
    h_rotation_ = 0.0;
    v_rotation_ = glm::radians(20.0);
    distance_ = 50.0;
}

void Camera::UpdateCamera(std::shared_ptr<Mouse> mouseCtx,
                  std::shared_ptr<Keyboard> keyCtx, bool isWindowFocused) {

    const static double ROTATION_DIFF_RATIO = 0.01;
    const static double SHIFT_DIFF_RATIO = 0.1;
    const static double SCROLL_RATIO = 1.1;

    if (!isWindowFocused) {
        if (mouseCtx->leftButton) {
            double xDiff = mouseCtx->deltaX;
            double yDiff = mouseCtx->deltaY;

            if (keyCtx->shift) {
                xDiff *= SHIFT_DIFF_RATIO;
                yDiff *= SHIFT_DIFF_RATIO;

                glm::mat4 r(1.0);
                r *= glm::rotate(glm::mat4(1.0), h_rotation_,
                                 glm::vec3(0.0, 0.0, 1.0));
                r *= glm::rotate(glm::mat4(1.0), -v_rotation_,
                                 glm::vec3(0.0, 1.0, 0.0));
                glm::vec4 shift(0.0, -(float)xDiff, (float)yDiff, 1.0);
                center_ += glm::vec3(r * shift);
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

glm::vec3 Camera::GeneratePos() {
    glm::vec3 pos;
    pos.x = distance_ * std::cos(h_rotation_) * std::cos(v_rotation_);
    pos.y = distance_ * std::sin(h_rotation_) * std::cos(v_rotation_);
    pos.z = distance_ * std::sin(v_rotation_);
    pos += center_;
    return pos;
}

glm::mat4 Camera::GenerateViewMat() {
    return glm::lookAt(GeneratePos(), center_, glm::vec3(0.0f, 0.0f, 1.0f));
}
