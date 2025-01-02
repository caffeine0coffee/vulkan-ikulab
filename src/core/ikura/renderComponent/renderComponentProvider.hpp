#pragma once

#include "../engine/renderEngine/renderEngine.hpp"

namespace ikura {
class RenderComponentProvider
    : public std::enable_shared_from_this<RenderComponentProvider> {
  protected:
    std::shared_ptr<RenderEngine> renderEngine;
    vk::DescriptorSetLayout descriptorSetLayout;

  public:
    RenderComponentProvider(const std::shared_ptr<RenderEngine> renderEngine);
    virtual ~RenderComponentProvider();
};
} // namespace ikura