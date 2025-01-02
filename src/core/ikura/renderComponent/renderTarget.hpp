#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <filesystem>

#include <vulkan/vulkan.hpp>

#include "../engine/renderEngine/renderEngine.hpp"
#include "./renderComponentProvider.hpp"

namespace ikura {
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
  protected:
    // Variables ==========
    std::shared_ptr<RenderEngine> renderEngine;

    // Basic objects for render ----------
    std::vector<vk::CommandBuffer> renderCmdBuffers;
    vk::PipelineLayout graphicsPipelineLayout;
    vk::Pipeline graphicsPipeline;
    vk::RenderPass renderPass;
    std::vector<vk::Framebuffer> frameBuffers;

    // Sync objects ----------
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> renderingFence;

    // ImageResources ----------
    ImageResource colorImageResource;
    ImageResource depthImageResource;
    std::vector<ImageResource> renderImageResources;

    // Properties ----------
    vk::Format colorImageFormat;
    vk::Extent2D imageExtent;
    vk::DescriptorSetLayout descriptorSetLayout;
    int numOfFrames;
    int numOfColorImages;

    // Methods ==========
    virtual void createSyncObjects();
    virtual void createRenderCmdBuffers();

    // helper functions ----------
    static vk::Format
    findDepthFormat(std::shared_ptr<RenderEngine> renderEngine);
    static void createImage(
        ImageResource &imageResource, const vk::Extent2D imageExtent,
        const uint32_t mipLevels, const vk::SampleCountFlagBits numSamples,
        const vk::Format format, const vk::ImageTiling tiling,
        const vk::ImageUsageFlags usage,
        const vk::MemoryPropertyFlags properties, VmaAllocator &allocator);
    static void createImageView(ImageResource &imageResource,
                                const vk::Format format,
                                const vk::ImageAspectFlags aspectFlags,
                                const uint32_t mipLevels,
                                const vk::Device device);
    static vk::ShaderModule
    createShaderModuleFromFile(const std::filesystem::path fileName,
                               const vk::Device device);
    
    static vk::ShaderModule createBasicVertexShaderModule(const vk::Device& device);
    static vk::ShaderModule createBasicFragmentShaderModule(const vk::Device& device);

  public:
    RenderTarget(const std::shared_ptr<RenderEngine> renderEngine,
                 vk::Format colorImageFormat, vk::Extent2D imageExtent,
                 vk::DescriptorSetLayout descriptorSetLayout,
                 std::vector<vk::Image> &renderImages, int numOfFrames);
    virtual ~RenderTarget();

    virtual void recreateResourcesForSwapChainRecreation(
        vk::Extent2D imageExtent, std::vector<vk::Image> renderImages);

    // Getters ----------
    vk::CommandBuffer &getRenderCommandBuffer(int index);

    vk::Semaphore &getImageAvailableSemaphore(int index);
    vk::Semaphore &getRenderFinishedSemaphore(int index);
    vk::Fence &getRenderingFence(int index);

    const vk::RenderPass &getRenderPass() const;
    const vk::Framebuffer &getFramebuffer(int imageIndex) const;
    const vk::Pipeline &getGraphicsPipeline() const;
    const vk::PipelineLayout &getGraphicsPipelineLayout() const;

    // Destroyers ----------
    void destroyResourcesForSwapChainRecreation();
};
} // namespace ikura