#include "./renderEngine.hpp"

#include <vulkan/vulkan.hpp>

#include <easylogging++.h>

#include "../../common/logLevels.hpp"

namespace ikura {
/**
 * @brief debug callback for ValidationLayer. prints error content e.g. error
 * message.
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {

    LOG(WARNING) << "ValidationLayer message: " << pCallbackData->pMessage;
    return VK_FALSE;
}

/**
 * @brief Populates vk::DebugUtilsMessengerCreateInfoEXT and return it.
 */
vk::DebugUtilsMessengerCreateInfoEXT RenderEngine::getDebugUtilsMessengerCI() {
    vk::DebugUtilsMessengerCreateInfoEXT ci;

    ci.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    ci.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
    ci.pfnUserCallback = validationLayerDebugCallback;

    return ci;
}

/**
 * @brief setup Extensions: DebugUtilsMessengerEXT
 */
void RenderEngine::setupExtensions() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Setting up Extensions...";

    if (isValidationLayerEnabled) {
        auto debugCI = getDebugUtilsMessengerCI();
        debugMessenger = instance.createDebugUtilsMessengerEXT(debugCI);
    }

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Vulkan Extensions have been setup.";
}

/**
 * @brief destroy Extensions: DebugUtilsMessengerEXT
 */
void RenderEngine::destroyExtensions() {
    if (isValidationLayerEnabled) {
        instance.destroyDebugUtilsMessengerEXT(debugMessenger);
    }
}
} // namespace ikura