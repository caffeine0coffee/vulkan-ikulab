#pragma once

#include <glm/glm.hpp>

namespace ikura {
const int NUM_OF_MODEL_MATRIX = 256;

struct BasicModelMatUBO {
    alignas(16) glm::mat4 model[NUM_OF_MODEL_MATRIX];
};

struct BasicSceneMatUBO {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};
} // namespace ikura