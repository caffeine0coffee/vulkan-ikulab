#include "renderEngine.hpp"

#include <easylogging++.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "common/logLevels.hpp"

namespace ikura {
// forward declearation of helper functions ----------
void checkLayersSupport(std::vector<const char *> &layersNames);
void checkInstanceExtensionsSupport(
    const std::vector<const char *> &extensionNames);
std::vector<const char *> getGlfwRequiredExtensions();

/**
 * @brief Populates CreateInfo, checks supports, and creates VulkanInstance.
 */
void RenderEngine::createInstance() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Creating Vulkan Instance...";

    vk::InstanceCreateInfo instanceCI{};
#if defined(__APPLE__)
    instanceCI.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

    // application info ----------
    vk::ApplicationInfo appInfo;
    appInfo.pApplicationName = initConfig.applicationName;
    appInfo.applicationVersion = initConfig.applicationVersion;
    appInfo.pEngineName = IKURA_APP_INFO_ENGINE_NAME;
    appInfo.engineVersion = IKURA_APP_INFO_ENGINE_VER;
    appInfo.apiVersion = IKURA_APP_INFO_API_VER;
    instanceCI.pApplicationInfo = &appInfo;

    // set Layer / Extension names ----------
    layerNames = initConfig.layerNames;
    instanceExtensionNames = initConfig.instanceExtensionNames;
    // for GLFW
    {
        auto glfwReqExts = getGlfwRequiredExtensions();
        std::for_each(
            glfwReqExts.begin(), glfwReqExts.end(), [&](const char *ext) {
                auto f =
                    std::find_if(instanceExtensionNames.begin(),
                                 instanceExtensionNames.end(),
                                 [&ext](const char *instanceExt) {
                                     return std::strcmp(ext, instanceExt) == 0;
                                 });
                if (f == instanceExtensionNames.end()) {
                    instanceExtensionNames.push_back(ext);
                }
            });
    }

    // Layer / Extension info population ----------
    instanceCI.enabledLayerCount = static_cast<uint32_t>(layerNames.size());
    instanceCI.ppEnabledLayerNames = layerNames.data();
    instanceCI.enabledExtensionCount =
        static_cast<uint32_t>(instanceExtensionNames.size());
    instanceCI.ppEnabledExtensionNames = instanceExtensionNames.data();

    // support check ----------
    // Layers
    checkLayersSupport(layerNames);
    if (std::find_if(layerNames.begin(), layerNames.end(),
                     [](const char *name) {
                         return std::strcmp(name, VALIDATION_LAYER_NAME) == 0;
                     }) != layerNames.end()) {

        isValidationLayerEnabled = true;
        LOG(INFO) << "Set ValidationLayer '" << VALIDATION_LAYER_NAME
                  << "' enabled.";
    }
    // InstanceExtensions
    checkInstanceExtensionsSupport(instanceExtensionNames);
    // Glfw
    // terminate program when GlfwNativeWindow creation is requested.
    engineInfo.support.isGlfwSupported = (glfwVulkanSupported() == GLFW_TRUE);
    if (engineInfo.support.isGlfwSupported) {
        LOG(INFO) << "Glfw is supported.";
    } else {
        LOG(WARNING) << "Glfw is NOT supported.";
    }

    // DebugUtils setting ----------
    vk::DebugUtilsMessengerCreateInfoEXT debugCI;
    if (isValidationLayerEnabled) {
        instanceCI.enabledLayerCount = static_cast<uint32_t>(layerNames.size());
        instanceCI.ppEnabledLayerNames = layerNames.data();
        debugCI = getDebugUtilsMessengerCI();

        instanceCI.pNext = &debugCI;
    } else {
        instanceCI.enabledLayerCount = 0;
        instanceCI.pNext = nullptr;
    }

    instance = vk::createInstance(instanceCI);

    // Initialize Vulkan Hpp Default DIspatcher
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Vulkan Instance has been created.";
}

/**
 * @brief Check if all of givin Layers are supported.
 * Prints INFO log and can throw runtime_error.
 *
 * @exception std::runtime_error if Layer is not supported.
 */
void checkLayersSupport(std::vector<const char *> &LayerNames) {
    // std::vector<vk::LayerProperties> availableLayers =
    // vk::enumerateInstanceLayerProperties();
    uint32_t count;
    std::vector<VkLayerProperties> availableLayers;
    vk::defaultDispatchLoaderDynamic.vkEnumerateInstanceLayerProperties(
        &count, nullptr);
    availableLayers.resize(count);
    vk::defaultDispatchLoaderDynamic.vkEnumerateInstanceLayerProperties(
        &count, availableLayers.data());

    if (VLOG_IS_ON(VLOG_LV_6_ITEM_ENUMERATION)) {
        VLOG(VLOG_LV_6_ITEM_ENUMERATION) << "Available Layers:";
        for (const auto &layer : availableLayers) {
            VLOG(VLOG_LV_6_ITEM_ENUMERATION) << "\t" << layer.layerName;
        }
    }

    bool supported = true;
    for (const auto &layerName : LayerNames) {
        supported =
            std::any_of(availableLayers.begin(), availableLayers.end(),
                        [&layerName](const vk::LayerProperties &prop) {
                            return std::strcmp(layerName, prop.layerName) == 0;
                        });

        if (!supported) {
            std::string msg;
            msg += "Layer activation is requested, but '";
            msg += layerName;
            msg += "' is NOT supported.";
            msg += "Make sure Layer name is correct.";
            LOG(ERROR) << msg;
            throw std::runtime_error(msg);
        }
    }

    LOG(INFO) << "All givin Layers are supported.";
}

/**
 * @brief Check if all of givin InstanceExtensions are supported.
 */
void checkInstanceExtensionsSupport(
    const std::vector<const char *> &extensionNames) {
    std::vector<const char *> required = extensionNames;
    auto requiredEndIter = required.end();

    // TODO: layerNameいらない
    auto checkExtensionProps = [&required,
                                &requiredEndIter](const char *layerName) {
        auto exProps = vk::enumerateInstanceExtensionProperties();

        if (layerName == nullptr) {
            VLOG(VLOG_LV_6_ITEM_ENUMERATION) << "\tGlobal:";
        } else {
            VLOG(VLOG_LV_6_ITEM_ENUMERATION)
                << "\tExtensions associated with layer '" << layerName << "':";
        }

        if (exProps.empty()) {
            VLOG(VLOG_LV_6_ITEM_ENUMERATION) << "\t\t(None)";
        }
        for (const auto &exProp : exProps) {
            VLOG(VLOG_LV_6_ITEM_ENUMERATION)
                << "\t\t" << exProp.extensionName
                << " version: " << exProp.specVersion;

            requiredEndIter = std::remove_if(
                required.begin(), requiredEndIter, [&exProp](const char *name) {
                    return std::strcmp(name, exProp.extensionName) == 0;
                });
        }
    };

    VLOG(VLOG_LV_6_ITEM_ENUMERATION) << "InstanceExtensions:";
    checkExtensionProps(nullptr);

    auto layerProps = vk::enumerateInstanceLayerProperties();

    for (const auto &layerProp : layerProps) {
        checkExtensionProps(layerProp.layerName);
    }

    if (requiredEndIter != required.begin()) {
        std::string msg;
        msg += "InstanceExtension activation is requested, ";
        msg += "but following Extensions are NOT supported: ";
        std::for_each(required.begin(), requiredEndIter,
                      [&msg](const char *name) {
                          msg += name;
                          msg += " ";
                      });
        msg += ". Make sure  name is correct.";
        LOG(ERROR) << msg;
        throw std::runtime_error(msg);
    }

    LOG(INFO) << "All givin InstanceExtensions are supported.";
}

/**
 * @brief Returns list of extension names that Glfw requires.
 */
std::vector<const char *> getGlfwRequiredExtensions() {
    uint32_t extCount;
    const char **exts = glfwGetRequiredInstanceExtensions(&extCount);

    std::vector<const char *> result;
    for (size_t i = 0; i < extCount; i++) {
        result.push_back(exts[i]);
    }

    return result;
}
} // namespace ikura
