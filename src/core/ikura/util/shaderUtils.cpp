//
// Created by kota.miyakawa on 2024/07/07.
//

#include "shaderUtils.hpp"

#include <string>

#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <vulkan/vulkan.hpp>

namespace ikura {
std::vector<uint32_t> compileShader(const std::string &source,
                                    const EShLanguage &stage) {
    glslang::InitializeProcess();

    const char *shaderStrings[1];
    shaderStrings[0] = source.c_str();

    glslang::TShader shader(stage);
    shader.setStrings(shaderStrings, 1);

    auto messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
    if (!shader.parse(GetDefaultResources(), 100, false, messages)) {
        // parse failed
        return {};
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(messages)) {
        // link failed
        return {};
    }

    std::vector<uint32_t> spirv;
    glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

    glslang::FinalizeProcess();
    return spirv;
}

vk::ShaderModule createShaderModule(const std::vector<uint32_t> &spirv,
                                    const vk::Device &device) {
    glslang::InitializeProcess();

    vk::ShaderModuleCreateInfo createInfo;
    createInfo.codeSize = spirv.size() * sizeof(uint32_t);
    createInfo.pCode = spirv.data();

    glslang::FinalizeProcess();
    return device.createShaderModule(createInfo);
}
} // namespace ikura
