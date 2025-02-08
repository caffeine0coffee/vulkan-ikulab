#include "imGuiVirtualWindow.hpp"

#include <filesystem>

#include <ikura_ext_imgui/imgui.h>
#include <ikura_ext_imgui/imgui_impl_glfw.h>
#include <ikura_ext_imgui/imgui_impl_vulkan.h>

namespace ikura {
void ImGuiVirtualWindow::initImGuiResources(
    ImGuiVirtualWindowInitConfig *initConfig) {
    setCurrentImGuiContext();

    // create descriptor pool
    vk::DescriptorPoolSize poolSizes[] = {
        {vk::DescriptorType::eSampler, 1000},
        {vk::DescriptorType::eCombinedImageSampler, 1000},
        {vk::DescriptorType::eSampledImage, 1000},
        {vk::DescriptorType::eStorageImage, 1000},
        {vk::DescriptorType::eUniformTexelBuffer, 1000},
        {vk::DescriptorType::eStorageTexelBuffer, 1000},
        {vk::DescriptorType::eUniformBuffer, 1000},
        {vk::DescriptorType::eStorageBuffer, 1000},
        {vk::DescriptorType::eUniformBufferDynamic, 1000},
        {vk::DescriptorType::eStorageBufferDynamic, 1000},
        {vk::DescriptorType::eInputAttachment, 1000}};

    vk::DescriptorPoolCreateInfo poolCI{};
    poolCI.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    poolCI.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
    poolCI.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
    poolCI.pPoolSizes = poolSizes;

    imGuiDescriptorPool =
        renderEngine->getDevice().createDescriptorPool(poolCI);

    ImGui_ImplGlfw_InitForVulkan(nativeWindow->getGLFWWindow(), true);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = (VkInstance)renderEngine->getInstance();
    initInfo.PhysicalDevice =
        (VkPhysicalDevice)renderEngine->getPhysicalDevice();
    initInfo.Device = (VkDevice)renderEngine->getDevice();
    initInfo.QueueFamily = renderEngine->getQueueFamilyIndices().get(
        ikura::QueueFamilyIndices::GRAPHICS);
    initInfo.Queue = (VkQueue)renderEngine->getQueues().graphicsQueue;
    initInfo.DescriptorPool = (VkDescriptorPool)imGuiDescriptorPool;
    initInfo.MinImageCount = 3;
    initInfo.ImageCount = 3;
    initInfo.MSAASamples = (VkSampleCountFlagBits)renderEngine->getEngineInfo()
                               .limit.maxMsaaSamples;
    initInfo.RenderPass = (VkRenderPass)nativeWindow->getRenderTarget()->getRenderPass();

    ImGui_ImplVulkan_Init(&initInfo);

    // Upload default font
    ImGuiIO &io = ImGui::GetIO();

    if (initConfig) {
        io.Fonts->AddFontFromFileTTF(initConfig->fontFilePath,
                                     initConfig->fontSizePixels, nullptr,
                                     io.Fonts->GetGlyphRangesJapanese());
    }

    //    auto cmd = renderEngine->beginSingleTimeCommands();
    //    ImGui_ImplVulkan_CreateFontsTexture((VkCommandBuffer)cmd);
    //    renderEngine->endSingleTimeCommands(cmd);
    //
    //    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiVirtualWindow::destroyImGuiResources() {
    setCurrentImGuiContext();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    renderEngine->getDevice().destroyDescriptorPool(imGuiDescriptorPool);
}

ImGuiVirtualWindow::ImGuiVirtualWindow(
    std::shared_ptr<RenderEngine> renderEngine,
    std::shared_ptr<GlfwNativeWindow> nativeWindow,
    ImGuiVirtualWindowInitConfig *initConfig)
    : VirtualWindow(renderEngine) {

    this->nativeWindow = nativeWindow;

    IMGUI_CHECKVERSION();

    imGuiContext = ImGui::CreateContext();
    initImGuiResources(initConfig);
}

ImGuiVirtualWindow::~ImGuiVirtualWindow() { destroyImGuiResources(); }

void ImGuiVirtualWindow::recordCommandBuffer(vk::CommandBuffer cmdBuffer) {
    setCurrentImGuiContext();

    ImDrawData *drawData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(drawData, (VkCommandBuffer)cmdBuffer);
}

bool ImGuiVirtualWindow::isFocused() const {
    setCurrentImGuiContext();
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

void ImGuiVirtualWindow::setCurrentImGuiContext() const {
    ImGui::SetCurrentContext(imGuiContext);
}

void ImGuiVirtualWindow::newFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
} // namespace ikura