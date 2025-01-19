#include "renderTarget.hpp"

#include <array>

#include <easylogging++.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "../window/nativeWindow/nativeWindow.hpp"

#include "../common/logLevels.hpp"
#include "../misc/shaderCodes.hpp"

#include <tinyfiledialogs/tinyfiledialogs.h>

namespace ikura {
void ImageResource::release(vk::Device device, VmaAllocator allocator) {
    if (releaseImageView) {
        device.destroyImageView(view);
    }
    if (releaseImage) {
        if (allocation.has_value()) {
            vmaDestroyImage(allocator, (VkImage)image, allocation.value());
        } else {
            device.destroyImage(image, nullptr);
        }
    }
}

void RenderTarget::createSyncObjects() {
    imageAvailableSemaphores.resize(numOfFrames);
    renderFinishedSemaphores.resize(numOfFrames);
    renderingFence.resize(numOfFrames);

    vk::SemaphoreCreateInfo semaphoreCI{};
    vk::FenceCreateInfo fenceCI{};
    fenceCI.flags = vk::FenceCreateFlagBits::eSignaled;

    for (int i = 0; i < numOfFrames; i++) {
        imageAvailableSemaphores[i] =
            renderEngine->getDevice().createSemaphore(semaphoreCI);
        renderFinishedSemaphores[i] =
            renderEngine->getDevice().createSemaphore(semaphoreCI);
        renderingFence[i] = renderEngine->getDevice().createFence(fenceCI);
    }
}

void RenderTarget::createRenderCmdBuffers() {
    renderCmdBuffers.resize(numOfFrames);

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = renderEngine->getCommandPool();
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = renderCmdBuffers.size();

    renderCmdBuffers =
        renderEngine->getDevice().allocateCommandBuffers(allocInfo);
}

void RenderTarget::recreateResourcesForSwapChainRecreation(
    vk::Extent2D imageExtent, std::vector<vk::Image> renderImages) {
}

vk::CommandBuffer &RenderTarget::getRenderCommandBuffer(int index) {
    return renderCmdBuffers[index];
}

vk::Semaphore &RenderTarget::getImageAvailableSemaphore(int index) {
    return imageAvailableSemaphores[index];
}

vk::Semaphore &RenderTarget::getRenderFinishedSemaphore(int index) {
    return renderFinishedSemaphores[index];
}

vk::Fence &RenderTarget::getRenderingFence(int index) {
    return renderingFence[index];
}

const vk::RenderPass &RenderTarget::getRenderPass() const { return renderPass; }

const vk::Framebuffer &RenderTarget::getFramebuffer(int imageIndex) const {
    return frameBuffers[imageIndex];
}

const vk::Pipeline &RenderTarget::getGraphicsPipeline() const {
    return graphicsPipeline;
}

const vk::PipelineLayout &RenderTarget::getGraphicsPipelineLayout() const {
    return graphicsPipelineLayout;
}

void RenderTarget::destroyResourcesForSwapChainRecreation() {
    colorImageResource.release(renderEngine->getDevice(),
                               *renderEngine->getVmaAllocator());
    depthImageResource.release(renderEngine->getDevice(),
                               *renderEngine->getVmaAllocator());
    for (auto &renderImageResource : renderImageResources) {
        renderImageResource.release(renderEngine->getDevice(),
                                    *renderEngine->getVmaAllocator());
    }

    for (auto &frameBuffer : frameBuffers) {
        renderEngine->getDevice().destroyFramebuffer(frameBuffer);
    }
    renderEngine->getDevice().destroyPipeline(graphicsPipeline);
    renderEngine->getDevice().destroyPipelineLayout(graphicsPipelineLayout);
    renderEngine->getDevice().destroyRenderPass(renderPass);
}

/// passed renderImages will not be released.
RenderTarget::RenderTarget(const std::shared_ptr<RenderEngine> renderEngine,
                           vk::Format colorImageFormat,
                           vk::Extent2D imageExtent,
                           vk::DescriptorSetLayout descriptorSetLayout,
                           std::vector<vk::Image> &renderImages,
                           int numOfFrames) {
    this->renderEngine = renderEngine;
    this->colorImageFormat = colorImageFormat;
    this->imageExtent = imageExtent;
    this->numOfColorImages = renderImages.size();
    this->numOfFrames = numOfFrames;
    this->descriptorSetLayout = descriptorSetLayout;

    // init renderImageResources with renderImages
    renderImageResources.resize(numOfColorImages);
    for (int i = 0; i < numOfColorImages; i++) {
        renderImageResources[i].image = renderImages[i];
        RenderTarget::createImageView(renderImageResources[i], colorImageFormat,
                                      vk::ImageAspectFlagBits::eColor, 1,
                                      renderEngine->getDevice());
        renderImageResources[i].releaseImage = false;
    }

    createSyncObjects();
    createRenderCmdBuffers();
}

RenderTarget::~RenderTarget() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying default GraphicsPipelne...";
    renderEngine->getDevice().destroyPipeline(graphicsPipeline);
    renderEngine->getDevice().destroyPipelineLayout(graphicsPipelineLayout);
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default GraphicsPipeline has been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying default FrameBuffers...";
    for (const auto &fBuffer : frameBuffers) {
        renderEngine->getDevice().destroyFramebuffer(fBuffer, nullptr);
    }
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default FrameBuffers has been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying ImageResources...";
    colorImageResource.release(renderEngine->getDevice(),
                               *renderEngine->getVmaAllocator());
    depthImageResource.release(renderEngine->getDevice(),
                               *renderEngine->getVmaAllocator());
    std::for_each(renderImageResources.begin(), renderImageResources.end(),
                  [&](ImageResource &ir) {
                      ir.release(renderEngine->getDevice(), nullptr);
                  });
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "ImageResources has been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying RenderPass...";
    renderEngine->getDevice().destroyRenderPass(renderPass);
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "RenderPass has been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying sync objects...";
    int numOfFrame = imageAvailableSemaphores.size();
    for (int i = 0; i < numOfFrame; i++) {
        renderEngine->getDevice().destroySemaphore(imageAvailableSemaphores[i]);
        renderEngine->getDevice().destroySemaphore(renderFinishedSemaphores[i]);
        renderEngine->getDevice().destroyFence(renderingFence[i]);
    }
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Sync objects have been destroyed.";
}

// Static functions ----------
vk::Format
RenderTarget::findDepthFormat(std::shared_ptr<RenderEngine> renderEngine) {
    // TODO: Allow changing
    auto tiling = vk::ImageTiling::eOptimal;
    auto feature = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

    const std::array<vk::Format, 3> candidates = {vk::Format::eD32Sfloat,
                                                  vk::Format::eD32SfloatS8Uint,
                                                  vk::Format::eD24UnormS8Uint};

    for (const auto &format : candidates) {
        auto props =
            renderEngine->getPhysicalDevice().getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear) {
            if ((props.linearTilingFeatures & feature) == feature) {
                return format;
            }
        } else if (tiling == vk::ImageTiling::eOptimal) {
            if ((props.optimalTilingFeatures & feature) == feature) {
                return format;
            }
        }
    }

    throw std::runtime_error(
        "Failed to find supported depth attachment format.");
}

void RenderTarget::createImage(
    ImageResource &imageResource, const vk::Extent2D imageExtent,
    const uint32_t mipLevels, const vk::SampleCountFlagBits numSamples,
    const vk::Format format, const vk::ImageTiling tiling,
    const vk::ImageUsageFlags usage, const vk::MemoryPropertyFlags properties,
    VmaAllocator &allocator) {

    vk::ImageCreateInfo imageCI{};
    imageCI.imageType = vk::ImageType::e2D;
    imageCI.extent = vk::Extent3D(imageExtent, 1);
    imageCI.mipLevels = mipLevels;
    imageCI.arrayLayers = 1;
    imageCI.format = format;
    imageCI.tiling = tiling;
    imageCI.initialLayout = vk::ImageLayout::eUndefined;
    imageCI.usage = usage;
    imageCI.samples = numSamples;
    imageCI.sharingMode = vk::SharingMode::eExclusive;

    VmaAllocationCreateInfo allocCI{};
    allocCI.usage = VMA_MEMORY_USAGE_AUTO;
    allocCI.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCI.priority = 1.0f;

    auto vkImageCI = (VkImageCreateInfo)imageCI;
    VkImage vkImage;
    VmaAllocation allocation;

    vmaCreateImage(allocator, &vkImageCI, &allocCI, &vkImage, &allocation,
                   nullptr);

    imageResource.image = (vk::Image)vkImage;
    imageResource.allocation = allocation;
}

void RenderTarget::createImageView(ImageResource &imageResource,
                                   const vk::Format format,
                                   const vk::ImageAspectFlags aspectFlags,
                                   const uint32_t mipLevels,
                                   const vk::Device device) {
    vk::ImageViewCreateInfo viewCI;
    viewCI.image = imageResource.image;
    viewCI.viewType = vk::ImageViewType::e2D;
    viewCI.format = format;

    viewCI.subresourceRange.aspectMask = aspectFlags;
    viewCI.subresourceRange.baseMipLevel = 0;
    viewCI.subresourceRange.levelCount = mipLevels;
    viewCI.subresourceRange.baseArrayLayer = 0;
    viewCI.subresourceRange.layerCount = 1;

    imageResource.view = device.createImageView(viewCI, nullptr);
}

vk::ShaderModule
RenderTarget::createShaderModuleFromFile(const std::filesystem::path fileName,
                                         const vk::Device device) {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::string msg = "";
        msg += "failed to open file '";
        msg += fileName.string();
        msg += "'.";
        throw std::runtime_error(msg);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    vk::ShaderModuleCreateInfo shaderModuleCI{};
    shaderModuleCI.codeSize = buffer.size();
    shaderModuleCI.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

    vk::ShaderModule shaderModule =
        device.createShaderModule(shaderModuleCI, nullptr);

    return shaderModule;
}
} // namespace ikura