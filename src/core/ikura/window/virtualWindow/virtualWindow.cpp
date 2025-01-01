#include "./virtualWindow.hpp"

#include <vulkan/vulkan.hpp>

namespace ikura {
VirtualWindow::VirtualWindow(std::shared_ptr<RenderEngine> renderEngine) {
    this->renderEngine = renderEngine;
}

VirtualWindow::~VirtualWindow() {}

void VirtualWindow::recordCommandBuffer(vk::CommandBuffer) {}

bool VirtualWindow::isFocused() const { return false; }
} // namespace ikura