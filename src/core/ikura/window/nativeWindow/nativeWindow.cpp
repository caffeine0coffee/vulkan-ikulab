#include "nativeWindow.hpp"

#include <easylogging++.h>
#include <vulkan/vulkan.hpp>

#include "common/logLevels.hpp"
#include "window/virtualWindow/virtualWindow.hpp"

namespace ikura {
void NativeWindow::recreateSwapChain(bool destroyExistingResources) {}

void NativeWindow::destroySwapChain() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Destroying SwapChain for '" << name << "'...";
    renderEngine->getDevice().destroySwapchainKHR(swapChain);
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "SwapChain for '" << name << "' has been destroyed.";
}

void NativeWindow::destroySurface() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Destroying Surface for '" << name << "'...";
    renderEngine->getInstance().destroySurfaceKHR(surface);
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Surface for '" << name << "' has been destroyed.";
}

NativeWindow::~NativeWindow() {
    if (!resourceDestroyed) {
        destroyResources();
    }
}

void NativeWindow::draw() {}

void NativeWindow::addVirtualWindow(
    std::shared_ptr<VirtualWindow> virtualWindow) {
    virtualWindows.push_back(virtualWindow);
}

const vk::SwapchainKHR NativeWindow::getSwapChain() const { return swapChain; }

const vk::Format NativeWindow::getSwapChainFormat() const {
    return swapChainFormat;
}

const vk::Extent2D NativeWindow::getSwapChainExtent() const {
    return swapChainExtent;
}

const std::vector<vk::Image> &NativeWindow::getSwapChainImages() const {
    return swapChainImages;
}

const uint32_t NativeWindow::getCurrentFrameIndex() const {
    return currentFrame;
}

const std::vector<std::shared_ptr<VirtualWindow>> &
NativeWindow::getVirtualWindows() const {
    return virtualWindows;
}

const bool NativeWindow::getIsWindowSizeZero() const {
    return isWindowSizeZero;
}
} // namespace ikura
