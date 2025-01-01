#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "../../misc/initVulkanHppDispatchLoader.hpp"

#define VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
#define IKURA_APP_INFO_ENGINE_NAME "Ikura"
#define IKURA_APP_INFO_ENGINE_VER VK_MAKE_VERSION(1, 0, 0)
#define IKURA_APP_INFO_API_VER VK_API_VERSION_1_2

namespace ikura {
// Forward Declearration ----------
class RenderEngine;
class RenderTarget;

struct RenderEngineInfo {
    struct SupportInfo {
        bool isGlfwSupported;
    } support;

    struct LimitInfo {
        vk::SampleCountFlagBits maxMsaaSamples;
    } limit;
};

class QueueFamilyIndices {
    typedef uint32_t QueueIndexKey;
    std::map<const QueueIndexKey, std::optional<uint32_t>> indices;

  public:
    static const QueueIndexKey GRAPHICS = 0;
    static const QueueIndexKey PRESENT = 1;

    QueueFamilyIndices();

    uint32_t get(const QueueIndexKey key) const;
    void set(QueueIndexKey key, uint32_t value);

    std::set<uint32_t> generateUniqueSet();

    bool isComplete() const;
    bool isShareingIndexBetweenGraphicsAndPresent() const;
};

struct PhysicalDeviceInfo {
    vk::PhysicalDevice device;
    QueueFamilyIndices queueFamilyIndices;
};

struct RenderEngineInitConfig {
    // app info ----------
    const char *applicationName;
    uint32_t applicationVersion;

    // Layers / Extensions ----------
    std::vector<const char *> layerNames;
    std::vector<const char *> instanceExtensionNames;
    std::vector<const char *> deviceExtensionNames;

    // callbacks ----------
    std::function<vk::PhysicalDevice(const RenderEngine *,
                                     std::vector<vk::PhysicalDevice>)>
        suitablePhysicalDevicePicker;

    // Template providers ----------
    static RenderEngineInitConfig defaultDebugSetting();

  private:
    static RenderEngineInitConfig defaultCommonSetting();
};

class RenderEngine {
    // Variables ==========
    // Basic Vulkan objects ----------
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Instance instance;
    struct Queues {
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
    } queues;
    QueueFamilyIndices queueFamilyIndices;
    vk::CommandPool cmdPool;

    // Layer / Extension ----------
    std::vector<const char *> layerNames;
    std::vector<const char *> instanceExtensionNames;
    std::vector<const char *> deviceExtensionNames;
    vk::DebugUtilsMessengerEXT debugMessenger;
    bool isValidationLayerEnabled = false;

    // Property / Information ----------
    RenderEngineInitConfig initConfig;
    RenderEngineInfo engineInfo;

    // Misc ----------
    vk::SurfaceKHR sampleSurface; // for PhysicalDevice suitability evaluation
    std::shared_ptr<VmaAllocator> vmaAllocator;

    // Functions ==========
    // Destruction ----------
    void destroyExtensions();

    // Misc ----------
    static vk::DebugUtilsMessengerCreateInfoEXT getDebugUtilsMessengerCI();

  public:
    // Functions ==========
    // Constructor / Desctuctor ----------
    RenderEngine(RenderEngineInitConfig initConfig);
    ~RenderEngine();

    // Creation / Setup ----------
    void createInstance();
    void createDevice();
    void setupExtensions();

    // Command ----------
    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

    // Getter ----------
    const vk::Instance getInstance() const;
    const vk::PhysicalDevice getPhysicalDevice() const;
    const vk::Device getDevice() const;
    const RenderEngineInfo getEngineInfo() const;
    const QueueFamilyIndices getQueueFamilyIndices() const;
    const std::shared_ptr<VmaAllocator> getVmaAllocator() const;
    const vk::CommandPool getCommandPool() const;
    const Queues &getQueues() const;

    // Setter ----------
    void setSampleSurface(vk::SurfaceKHR surface);

    // Misc ----------
    // TODO: rename to getSuitablePhysicalDevice()
    static vk::PhysicalDevice
    getSuitablePhysicalDeviceInfo(const RenderEngine *pEngine,
                                  std::vector<vk::PhysicalDevice> devices);
    void waitForDeviceIdle();
};
} // namespace ikura