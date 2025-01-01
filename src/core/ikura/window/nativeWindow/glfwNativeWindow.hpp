#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "./nativeWindow.hpp"

namespace ikura {
class GlfwNativeWindow : public NativeWindow {
  protected:
    GLFWwindow *window;

    // TODO: make them NativeWindow:: protected member ---
    bool frameBufferResized = false;
    vk::SwapchainCreateInfoKHR swapChainCICache;
    // ---

    void createSwapChain();
    void recordCommandBuffer(uint32_t imageIndex);
    void recreateSwapChain(bool destroyExistingResources = true) override;

    void destroyGlfwWindow();

  public:
    GlfwNativeWindow(const std::shared_ptr<RenderEngine> renderEngine,
                     GLFWwindow *window, vk::SurfaceKHR surface,
                     std::string name);
    ~GlfwNativeWindow();

    float getScaleX() const override;
    float getScaleY() const override;

    void destroyResources() override;
    void draw() override;
    bool closed() override;

    GLFWwindow *getGLFWWindow() const;
};
} // namespace ikura
