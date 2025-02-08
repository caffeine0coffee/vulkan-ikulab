#include "camera.hpp"

void Camera::init() {
    center = {0.0, 0.0, 5.0};
    hRotation = 0.0;
    vRotation = glm::radians(20.0);
    distance = 50.0;
}

void Camera::updateCamera(std::shared_ptr<Mouse> mouseCtx,
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
                r *= glm::rotate(glm::mat4(1.0), hRotation,
                                 glm::vec3(0.0, 0.0, 1.0));
                r *= glm::rotate(glm::mat4(1.0), -vRotation,
                                 glm::vec3(0.0, 1.0, 0.0));
                glm::vec4 shift(0.0, -(float)xDiff, (float)yDiff, 1.0);
                center += glm::vec3(r * shift);
            } else {
                xDiff *= ROTATION_DIFF_RATIO;
                yDiff *= ROTATION_DIFF_RATIO;

                hRotation = std::fmod(hRotation - xDiff, 2 * M_PI);
                vRotation =
                    std::clamp(std::fmod(vRotation + yDiff, 2 * M_PI),
                               -M_PI / 2.0 + 0.0001, M_PI / 2.0 - 0.0001);
            }
        }
        distance *= std::pow(SCROLL_RATIO, -mouseCtx->scrollOffsetY);
    }
}

glm::vec3 Camera::generatePos() {
    glm::vec3 pos;
    pos.x = distance * std::cos(hRotation) * std::cos(vRotation);
    pos.y = distance * std::sin(hRotation) * std::cos(vRotation);
    pos.z = distance * std::sin(vRotation);
    pos += center;
    return pos;
}

glm::mat4 Camera::generateViewMat() {
    return glm::lookAt(generatePos(), center, glm::vec3(0.0f, 0.0f, 1.0f));
}
