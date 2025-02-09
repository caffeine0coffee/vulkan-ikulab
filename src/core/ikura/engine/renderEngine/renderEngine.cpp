#include "renderEngine.hpp"

// TODO(caffeine): move to dedicated file
// (see https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html)
#define VMA_IMPLEMENTATION

#include <easylogging++.h>
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>

#include "common/logLevels.hpp"

namespace ikura {
QueueFamilyIndices::QueueFamilyIndices() {
    indices.insert({std::make_pair(GRAPHICS, decltype(indices)::mapped_type()),
                    std::make_pair(PRESENT, decltype(indices)::mapped_type())});
}

uint32_t QueueFamilyIndices::get(const QueueIndexKey key) const {
    return indices.at(key).value();
}

void QueueFamilyIndices::set(QueueIndexKey key, uint32_t value) {
    indices.insert_or_assign(key, value);
}

std::set<uint32_t> QueueFamilyIndices::generateUniqueSet() {
    std::set<uint32_t> result;
    for (const auto &value : indices) {
        if (value.second.has_value()) {
            result.insert(value.second.value());
        }
    }

    return result;
}

bool QueueFamilyIndices::isComplete() const {
    return std::all_of(indices.begin(), indices.end(),
                       [](const decltype(indices)::value_type value) {
                           return value.second.has_value();
                       });
}

bool QueueFamilyIndices::isShareingIndexBetweenGraphicsAndPresent() const {
    return get(GRAPHICS) == get(PRESENT);
}

RenderEngine::RenderEngine(RenderEngineInitConfig initConfig) {
    glfwInit();
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Initialized GLFW.";
    initDispatchLoader();
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Initialized Vulkan Hpp Dispatch Loader.";

    this->initConfig = initConfig;
    vmaAllocator = std::make_shared<VmaAllocator>();
}

RenderEngine::~RenderEngine() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying CommandPool...";
    device.destroyCommandPool(cmdPool);
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "CommandPool has been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying VmaAllocator...";
    vmaDestroyAllocator(*vmaAllocator);
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "VmaAllocator has been Destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying Vulkan Device...";
    device.destroy();
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Vulkan Device has been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying Vulkan Extensions...";
    destroyExtensions();
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Vulkan Extensions have been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying Vulkan Instance...";
    instance.destroy();
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Vulkan Instance has been destroyed.";

    glfwTerminate();
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Terminated GLFW.";
}

vk::CommandBuffer RenderEngine::beginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = cmdPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer cmdBuffer;
    auto result = device.allocateCommandBuffers(&allocInfo, &cmdBuffer);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to allocate CommandBuffer.");
    }

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    result = cmdBuffer.begin(&beginInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to begin command buffer.");
    }

    return cmdBuffer;
}

void RenderEngine::endSingleTimeCommands(vk::CommandBuffer cmdBuffer) {
    cmdBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    queues.graphicsQueue.submit(submitInfo);
    queues.graphicsQueue.waitIdle();
    device.freeCommandBuffers(cmdPool, cmdBuffer);
}

const vk::Instance RenderEngine::getInstance() const { return instance; }

const vk::PhysicalDevice RenderEngine::getPhysicalDevice() const {
    return physicalDevice;
}

const vk::Device RenderEngine::getDevice() const { return device; }

const RenderEngineInfo RenderEngine::getEngineInfo() const {
    return engineInfo;
}

const QueueFamilyIndices RenderEngine::getQueueFamilyIndices() const {
    return queueFamilyIndices;
}

const std::shared_ptr<VmaAllocator> RenderEngine::getVmaAllocator() const {
    return vmaAllocator;
}

const vk::CommandPool RenderEngine::getCommandPool() const { return cmdPool; }

const RenderEngine::Queues &RenderEngine::getQueues() const { return queues; }

void RenderEngine::setSampleSurface(vk::SurfaceKHR surface) {
    this->sampleSurface = surface;
}

void RenderEngine::waitForDeviceIdle() { device.waitIdle(); }

RenderEngineInitConfig RenderEngineInitConfig::defaultDebugSetting() {
    RenderEngineInitConfig initConfig = defaultCommonSetting();

#if defined(IKURA_ENABLE_VALIDATION_LAYER)
    initConfig.layerNames.push_back(VALIDATION_LAYER_NAME);
#endif

#if defined(IKURA_ENABLE_VALIDATION_LAYER)
    initConfig.instanceExtensionNames.push_back(
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    initConfig.instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(__APPLE__)
    initConfig.instanceExtensionNames.push_back("VK_KHR_portability_enumeration");
#endif
    initConfig.deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#if defined(__APPLE__)
    initConfig.deviceExtensionNames.push_back("VK_KHR_portability_subset");
#endif

    return initConfig;
}

RenderEngineInitConfig RenderEngineInitConfig::defaultCommonSetting() {
    RenderEngineInitConfig initConfig;
    initConfig.applicationName = "Ikura Application";
    initConfig.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    initConfig.suitablePhysicalDevicePicker =
        RenderEngine::getSuitablePhysicalDeviceInfo;

    return initConfig;
}
} // namespace ikura
