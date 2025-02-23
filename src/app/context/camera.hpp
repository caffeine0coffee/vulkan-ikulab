#pragma once

#include <memory>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

#include "keyboard.hpp"
#include "mouse.hpp"

class Camera {
  public:
    glm::vec3 center_;
    /// in Radians
    float h_rotation_;
    /// in Radians
    float v_rotation_;
    float distance_;

    void init();
    void updateCamera(std::shared_ptr<Mouse> mouseCtx,
                      std::shared_ptr<Keyboard> keyCtx, bool isWindowFocused);

    glm::vec3 generatePos();
    glm::mat4 generateViewMat();
};
