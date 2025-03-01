#include "glfwNativeWindow.hpp"

#include <easylogging++.h>

#include "common/logLevels.hpp"
#include "window/virtualWindow/virtualWindow.hpp"

#if defined(IS_WINDOWS)
#include <climits>
#endif

// Forward declearation of helper functions ----------
vk::SurfaceFormatKHR
chooseSwapChainFormat(const std::vector<vk::SurfaceFormatKHR> &formats);
vk::PresentModeKHR
chooseSwapChainPresentMode(const std::vector<vk::PresentModeKHR> &presentModes);
vk::Extent2D
chooseSwapChainExtent(const vk::SurfaceCapabilitiesKHR &capabilities,
                      GLFWwindow *window);

namespace ikura {
/**
 * @brief Constructs ikura::GlfwNativeWindow.
 * If surface is nullptr, this function creates new vk::SurfaceKHR object.
 * The other hand, if surface is NOT nullptr, this function constructs
 * GlfwNativeWindow object from passed surface. The passed surface will be
 * destroyed automaticaly.
 */
GlfwNativeWindow::GlfwNativeWindow(
    const std::shared_ptr<RenderEngine> renderEngine,
    GLFWwindow *window = nullptr, vk::SurfaceKHR surface = nullptr,
    std::string name = nullptr) {

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Creating GlfwNativeWindow '" << name << "'...";
    this->renderEngine = renderEngine;
    this->name = name;

    // Set GLFW Basic Properties
    this->window = window;
    glfwGetFramebufferSize(this->window, &width, &height);

    // Create Surface
    if (surface) {
        this->surface = surface;
    } else {
        VkSurfaceKHR vkSurface;
        if ((glfwCreateWindowSurface((VkInstance)renderEngine->getInstance(),
                                     this->window, nullptr, &vkSurface)) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create VkSurfaceKHR from "
                                     "glfwCreateWindowSurface().");
        }
        surface = vk::SurfaceKHR(vkSurface);

        VLOG(VLOG_LV_3_PROCESS_TRACKING)
            << "New Surface for '" << name << "' has been created.";
    }

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Surface for '" << name << "' has been setup.";

    createSwapChain();
    swapChainImages =
        renderEngine->getDevice().getSwapchainImagesKHR(swapChain);
}

GlfwNativeWindow::~GlfwNativeWindow() {
    if (!resourceDestroyed) {
        destroyGlfwWindow();
    }
}

float GlfwNativeWindow::getScaleX() const {
    float xScale;
    glfwGetWindowContentScale(window, &xScale, nullptr);

    return xScale;
}

float GlfwNativeWindow::getScaleY() const {
    float yScale;
    glfwGetWindowContentScale(window, nullptr, &yScale);

    return yScale;
}

void GlfwNativeWindow::createSwapChain() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Creating SwapChain for '" << name << "'...";

    auto surfaceFormats =
        renderEngine->getPhysicalDevice().getSurfaceFormatsKHR(surface);
    auto surfacePresentModes =
        renderEngine->getPhysicalDevice().getSurfacePresentModesKHR(surface);
    auto surfaceCapabilities =
        renderEngine->getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);

    if (VLOG_IS_ON(VLOG_LV_6_ITEM_ENUMERATION)) {
        VLOG(VLOG_LV_6_ITEM_ENUMERATION) << "Available SwapChain formats:";
        for (const auto &format : surfaceFormats) {
            VLOG(VLOG_LV_6_ITEM_ENUMERATION)
                << "\t" << vk::to_string(format.format) << " / "
                << vk::to_string(format.colorSpace);
        }

        VLOG(VLOG_LV_6_ITEM_ENUMERATION)
            << "Available SwapChain present modes:";
        for (const auto &mode : surfacePresentModes) {
            VLOG(VLOG_LV_6_ITEM_ENUMERATION) << "\t" << vk::to_string(mode);
        }
    }

    vk::SurfaceFormatKHR format = chooseSwapChainFormat(surfaceFormats);
    vk::PresentModeKHR presentMode =
        chooseSwapChainPresentMode(surfacePresentModes);
    vk::Extent2D extent = chooseSwapChainExtent(surfaceCapabilities, window);
    this->width = extent.width;
    this->height = extent.height;

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Chose SwapChain format: " << vk::to_string(format.format) << " / "
        << vk::to_string(format.colorSpace);
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Chose SwapChain present mode: " << vk::to_string(presentMode);
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "SwapChain Extent: " << extent.width << "x" << extent.height;

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 &&
        imageCount > surfaceCapabilities.maxImageCount) {

        imageCount = surfaceCapabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapChainCI;
    swapChainCI.surface = surface;
    swapChainCI.minImageCount = imageCount;

    swapChainCI.imageFormat = format.format;
    swapChainCI.imageColorSpace = format.colorSpace;
    swapChainCI.imageExtent = extent;
    swapChainCI.imageArrayLayers = 1;
    swapChainCI.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    swapChainCI.preTransform = surfaceCapabilities.currentTransform;
    swapChainCI.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapChainCI.presentMode = presentMode;
    swapChainCI.clipped = VK_TRUE;

    const QueueFamilyIndices queueFamilyIndices =
        renderEngine->getQueueFamilyIndices();
    uint32_t indices[] = {queueFamilyIndices.get(QueueFamilyIndices::GRAPHICS),
                          queueFamilyIndices.get(QueueFamilyIndices::PRESENT)};

    if (queueFamilyIndices.isShareingIndexBetweenGraphicsAndPresent()) {
        swapChainCI.imageSharingMode = vk::SharingMode::eExclusive;
        swapChainCI.queueFamilyIndexCount = 0;
    } else {
        swapChainCI.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCI.queueFamilyIndexCount = 2;
        swapChainCI.pQueueFamilyIndices = indices;
    }

    swapChain = renderEngine->getDevice().createSwapchainKHR(swapChainCI);
    swapChainFormat = format.format;
    swapChainExtent = extent;
    swapChainCICache = swapChainCI;

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "SwapChain for '" << name << "' has been created.";
}

void GlfwNativeWindow::destroyResources() {
    destroyGlfwWindow();
    destroySwapChain();
    destroySurface();

    resourceDestroyed = true;
}

bool GlfwNativeWindow::closed() { return glfwWindowShouldClose(window) != 0; }

void GlfwNativeWindow::draw() {
    // If window size is zero, prevent drawing.
    // If window is restored from minimized, recreate swapchain and reset
    // states. `isWindowSizeRestoredFromMinimized` can be true by only GLFW
    // callback function.
    if (isWindowSizeZero) {
        if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            recreateSwapChain(false);
            isWindowSizeZero = false;
        } else {
            return;
        }
    }

    // Wait for previous frame to complete
    auto result = renderEngine->getDevice().waitForFences(
        renderTarget->getRenderingFence(currentFrame), VK_TRUE, UINT64_MAX);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Error occurred while waiting fence.");
    }
    renderEngine->getDevice().resetFences(
        renderTarget->getRenderingFence(currentFrame));

    // Acquire swapChain image
    auto nextImage = renderEngine->getDevice().acquireNextImageKHR(
        swapChain, UINT64_MAX,
        renderTarget->getImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE);

    if (nextImage.result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return;
    } else if (nextImage.result != vk::Result::eSuccess &&
               nextImage.result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire next SwapChain image.");
    }

    // Record command buffer
    renderTarget->getRenderCommandBuffer(currentFrame).reset({});
    recordCommandBuffer(nextImage.value);

    // Submit command buffer
    std::array<vk::PipelineStageFlags, 1> waitStages = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.setWaitSemaphores(
        renderTarget->getImageAvailableSemaphore(currentFrame));
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers =
        &renderTarget->getRenderCommandBuffer(currentFrame);
    submitInfo.setSignalSemaphores(
        renderTarget->getRenderFinishedSemaphore(currentFrame));

    renderEngine->getQueues().graphicsQueue.submit(
        submitInfo, renderTarget->getRenderingFence(currentFrame));

    // Presentation
    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.setWaitSemaphores(
        renderTarget->getRenderFinishedSemaphore(currentFrame));
    presentInfo.swapchainCount = 1;
    presentInfo.setSwapchains(swapChain);
    presentInfo.setImageIndices(nextImage.value);

    try {
        result = renderEngine->getQueues().presentQueue.presentKHR(presentInfo);
    } catch (vk::OutOfDateKHRError &e) {
        frameBufferResized = false;
        recreateSwapChain();
    } catch (vk::Error &e) {
        throw std::runtime_error("Failed to present swapChain image.");
    }

    if (result == vk::Result::eSuboptimalKHR || frameBufferResized) {
        frameBufferResized = false;
        recreateSwapChain();
    }

    currentFrame = (currentFrame + 1) % numOfFrames;
}

GLFWwindow *GlfwNativeWindow::getGLFWWindow() const { return window; }

void GlfwNativeWindow::recordCommandBuffer(uint32_t imageIndex) {
    // Begin ----------
    vk::CommandBufferBeginInfo beginInfo{};
    renderTarget->getRenderCommandBuffer(currentFrame).begin(beginInfo);

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color =
        vk::ClearColorValue(std::array<uint32_t, 4>{1, 0, 0, 0});
    clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.renderPass = renderTarget->getRenderPass();
    renderPassInfo.framebuffer = renderTarget->getFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D(0, 0);
    renderPassInfo.renderArea.extent = swapChainExtent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    renderTarget->getRenderCommandBuffer(currentFrame)
        .beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    // Bind objects ----------
    renderTarget->getRenderCommandBuffer(currentFrame)
        .bindPipeline(vk::PipelineBindPoint::eGraphics,
                      renderTarget->getGraphicsPipeline());
    renderTarget->getRenderCommandBuffer(currentFrame)
        .bindVertexBuffers(0, {renderContent->getVertexBuffer()}, {0});
    renderTarget->getRenderCommandBuffer(currentFrame)
        .bindIndexBuffer(renderContent->getIndexBuffer(), 0,
                         vk::IndexType::eUint32);
    renderTarget->getRenderCommandBuffer(currentFrame)
        .bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                            renderTarget->getGraphicsPipelineLayout(), 0,
                            renderContent->getDescriptorSets(currentFrame),
                            nullptr);

    // Draw ----------
    renderTarget->getRenderCommandBuffer(currentFrame)
        .drawIndexed(renderContent->getNumOfIndex(), 1, 0, 0, 0);

    // VirtualWindows ----------
    for (auto &vWindow : virtualWindows) {
        vWindow->recordCommandBuffer(
            renderTarget->getRenderCommandBuffer(currentFrame));
    }

    // End ----------
    renderTarget->getRenderCommandBuffer(currentFrame).endRenderPass();
    renderTarget->getRenderCommandBuffer(currentFrame).end();
}

void GlfwNativeWindow::recreateSwapChain(bool destroyExistingResources) {
    renderEngine->getDevice().waitIdle();

    if (destroyExistingResources) {
        // Clean up SwapChain
        renderTarget->destroyResourcesForSwapChainRecreation();
        renderEngine->getDevice().destroySwapchainKHR(swapChain);
    }

    // Recreate SwapChain ----------
    auto extent = chooseSwapChainExtent(
        renderEngine->getPhysicalDevice().getSurfaceCapabilitiesKHR(surface),
        window);

    // If window size is zero, prevent new resources creation because Vulkan
    //  cannot create zero-size images.
    // On Windows10, window size will be (0, 0) when window is minimized.
    // On Linux (X11), when window is minimized, window is just hidden
    //  and keep rendering on background.
    // (TODO: investigate on macOS, wayland, (maybe also Windows11 is needed))
    if (extent.width == 0 | extent.height == 0) {
        isWindowSizeZero = true;
    } else {
        isWindowSizeZero = false;
        vk::SwapchainCreateInfoKHR swapChainCI = swapChainCICache;
        swapChainCI.imageExtent = extent;
        swapChain = renderEngine->getDevice().createSwapchainKHR(swapChainCI);

        swapChainExtent = extent;
        swapChainImages =
            renderEngine->getDevice().getSwapchainImagesKHR(swapChain);
        width = extent.width;
        height = extent.height;

        renderTarget->recreateResourcesForSwapChainRecreation(swapChainExtent,
                                                              swapChainImages);
    }
}

void GlfwNativeWindow::destroyGlfwWindow() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Destroying GLFW Window for '" << name << "'...";
    glfwDestroyWindow(window);
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "GLFW Window for '" << name << "' has been destroyed.";
}
} // namespace ikura

vk::SurfaceFormatKHR
chooseSwapChainFormat(const std::vector<vk::SurfaceFormatKHR> &formats) {
    for (const auto &format : formats) {
        if (format.format == vk::Format::eR8G8B8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {

            return format;
        }
    }

    return formats[0];
}

vk::PresentModeKHR chooseSwapChainPresentMode(
    const std::vector<vk::PresentModeKHR> &presentModes) {
    for (const auto &presentMode : presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            return presentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D
chooseSwapChainExtent(const vk::SurfaceCapabilitiesKHR &capabilities,
                      GLFWwindow *window) {
#if defined(IS_WINDOWS)
    if (capabilities.currentExtent.width != INT_MAX) {
#else
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
#endif
        return capabilities.currentExtent;
    } else {
        int width, height;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
        glfwGetFramebufferSize(window, &width, &height);

        vk::Extent2D actualExtent = {static_cast<uint32_t>(width),
                                     static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
