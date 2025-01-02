//
// Created by kota.miyakawa on 2024/07/07.
//

#pragma once

#include <string>

#include <glslang/Public/ShaderLang.h>
#include <vulkan/vulkan.hpp>

namespace ikura {

std::vector<uint32_t> compileShader(const std::string &source,
                                                      const EShLanguage &stage);

vk::ShaderModule createShaderModule(const std::vector<uint32_t> &spirv,
                                                     const vk::Device &device);

} // namespace ikura