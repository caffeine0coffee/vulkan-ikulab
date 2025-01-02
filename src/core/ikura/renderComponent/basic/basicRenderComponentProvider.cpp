#include "./basicRenderComponentProvider.hpp"

#include <easylogging++.h>

#include "./descriptorSetProps.hpp"

#include "../../common/logLevels.hpp"

namespace ikura {
void BasicRenderComponentProvider::createDescriptorSetlayout() {
    vk::DescriptorSetLayoutBinding modelMatLayoutBinding{};
    modelMatLayoutBinding.binding = DESCRIPTOR_SET_BINDING_MODEL_MATRIX_UBO;
    modelMatLayoutBinding.descriptorCount = 1;
    modelMatLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    modelMatLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutBinding sceneMatLayoutBinding{};
    sceneMatLayoutBinding.binding = DESCRIPTOR_SET_BINDING_SCENE_MATRIX_UBO;
    sceneMatLayoutBinding.descriptorCount = 1;
    sceneMatLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    sceneMatLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    std::array<vk::DescriptorSetLayoutBinding, NUM_OF_DESCRIPTORS>
        layoutBindings = {modelMatLayoutBinding, sceneMatLayoutBinding};
    vk::DescriptorSetLayoutCreateInfo layoutCI{};
    layoutCI.bindingCount = layoutBindings.size();
    layoutCI.pBindings = layoutBindings.data();

    descriptorSetLayout =
        renderEngine->getDevice().createDescriptorSetLayout(layoutCI);
}

BasicRenderComponentProvider::BasicRenderComponentProvider(
    const std::shared_ptr<RenderEngine> renderEngine)
    : RenderComponentProvider(renderEngine) {

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Creating Basic DescriptorSetLayout.";
    createDescriptorSetlayout();
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Basic DescriptiorSetLayout has been created.";
}

BasicRenderComponentProvider::~BasicRenderComponentProvider() {}

std::shared_ptr<BasicRenderTarget>
BasicRenderComponentProvider::createBasicRenderTarget(
    const std::shared_ptr<NativeWindow> nativeWindow) {

    auto images = renderEngine->getDevice().getSwapchainImagesKHR(
        nativeWindow->getSwapChain());
    auto renderTarget = std::make_shared<BasicRenderTarget>(
        renderEngine, nativeWindow->getSwapChainFormat(),
        nativeWindow->getSwapChainExtent(), descriptorSetLayout, images,
        nativeWindow->getNumOfFrames());

    return renderTarget;
}

std::shared_ptr<BasicRenderContent>
BasicRenderComponentProvider::createBasicRenderContent(
    const std::shared_ptr<NativeWindow> nativeWindow) {

    auto renderContent = std::make_shared<BasicRenderContent>(
        renderEngine, descriptorSetLayout, nativeWindow->getNumOfFrames());

    return renderContent;
}

} // namespace ikura
