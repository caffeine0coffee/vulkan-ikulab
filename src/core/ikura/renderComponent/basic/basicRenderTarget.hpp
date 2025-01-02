#pragma once

#include "../renderTarget.hpp"

namespace ikura {
class BasicRenderTarget : public RenderTarget {
    void setupRenderPass();
    void setupImageResources();
    void setupFrameBuffers();
    void setupGraphicsPipeline();

  public:
    BasicRenderTarget(const std::shared_ptr<RenderEngine> renderEngine,
                      vk::Format colorImageFormat, vk::Extent2D imageExtent,
                      vk::DescriptorSetLayout descriptorSetLayout,
                      std::vector<vk::Image> &renderImages, int numOfFrames);

    void recreateResourcesForSwapChainRecreation(
        vk::Extent2D imageExtent, std::vector<vk::Image> renderImages) override;
};
} // namespace ikura