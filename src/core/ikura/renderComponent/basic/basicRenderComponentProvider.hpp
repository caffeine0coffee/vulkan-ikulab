#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>

#include "basicRenderContent.hpp"
#include "basicRenderTarget.hpp"
#include "engine/renderEngine/renderEngine.hpp"
#include "renderComponent/renderComponentProvider.hpp"
#include "window/nativeWindow/nativeWindow.hpp"

namespace ikura {
// Provides Basic RenderComponent compatible with shapes
class BasicRenderComponentProvider : public RenderComponentProvider {

    void createDescriptorSetlayout();

  public:
    BasicRenderComponentProvider(
        const std::shared_ptr<RenderEngine> renderEngine);
    ~BasicRenderComponentProvider();

    std::shared_ptr<BasicRenderTarget>
    createBasicRenderTarget(const std::shared_ptr<NativeWindow> nativeWindow);
    std::shared_ptr<BasicRenderContent>
    createBasicRenderContent(const std::shared_ptr<NativeWindow> nativeWindow);
};
} // namespace ikura