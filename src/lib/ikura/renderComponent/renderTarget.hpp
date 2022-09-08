#pragma once

#include <memory>
#include <vector>
#include <optional>

#include <vulkan/vulkan.hpp>

#include "../engine/renderEngine/renderEngine.hpp"

namespace ikura {
// Forward declearation ----------
class NativeWindow;

class ImageResource {
  public:
    vk::Image image;
    vk::ImageView view;
    std::optional<VmaAllocation> allocation;
    bool releaseImage = true;
    bool releaseImageView = true;

    void release(vk::Device device, VmaAllocator allocator);
};

class RenderTarget {
    // Variables ==========
    std::shared_ptr<RenderEngine> renderEngine;
    std::weak_ptr<NativeWindow> nativeWindow;

    // Basic objects for render ----------
    vk::CommandBuffer commandBuffer;
    std::vector<vk::PipelineLayout> graphicsPipelineLayouts;
    std::vector<vk::Pipeline> graphicsPipelines;
    vk::RenderPass renderPass;
    vk::Framebuffer frameBuffer;

    // ImageResource ----------
    ImageResource colorImageResource;
    ImageResource depthImageResource;
    std::vector<ImageResource> renderImageResources;

    // Descriptor ----------
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorPool descriptorPool;

    // Methods ==========
    // Creation of default resources ----------
    void createDefaultRenderPass();
    void createDefaultImageResources();

    // Init of default resources ----------
    void initRenderImageResourcesFromNativeWindow();

  public:
    RenderTarget(const std::shared_ptr<NativeWindow> nativeWindow,
                 const std::shared_ptr<RenderEngine> renderEngine);
    ~RenderTarget();

    void setDefaultResources();
};
} // namespace ikura