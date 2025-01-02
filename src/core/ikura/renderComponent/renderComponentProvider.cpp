#include "./renderComponentProvider.hpp"

#include <easylogging++.h>

#include "../common/logLevels.hpp"

namespace ikura {
RenderComponentProvider::RenderComponentProvider(
    const std::shared_ptr<RenderEngine> renderEngine) {

    this->renderEngine = renderEngine;
}

RenderComponentProvider::~RenderComponentProvider() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Desctroying DescriptorSetLayout...";
    renderEngine->getDevice().destroyDescriptorSetLayout(descriptorSetLayout);
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "DescriptorSetLayout has been deleted.";
}

} // namespace ikura