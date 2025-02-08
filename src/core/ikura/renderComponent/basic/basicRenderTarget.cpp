#include "basicRenderTarget.hpp"

#include <easylogging++.h>

#include "common/logLevels.hpp"
#include "common/renderPrimitiveTypes.hpp"
#include "misc/shaderCodes.hpp"
#include "util/shaderUtils.hpp"

namespace ikura {
void BasicRenderTarget::setupRenderPass() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Creating Default RenderPass...";

    // Attachment Descriptions ----------
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = colorImageFormat;
    colorAttachment.samples =
        renderEngine->getEngineInfo().limit.maxMsaaSamples;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = RenderTarget::findDepthFormat(renderEngine);
    depthAttachment.samples =
        renderEngine->getEngineInfo().limit.maxMsaaSamples;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout =
        vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = colorImageFormat;
    colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    // Attachment Refs ----------
    vk::AttachmentReference colorAttachmentRef{
        0, vk::ImageLayout::eColorAttachmentOptimal};
    vk::AttachmentReference depthAttachmentRef{
        1, vk::ImageLayout::eDepthStencilAttachmentOptimal};
    vk::AttachmentReference colorAttachmentResolveRef{
        2, vk::ImageLayout::eColorAttachmentOptimal};

    // Subpass / Dependency ----------
    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    vk::SubpassDependency dependency{};
    // source
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask =
        (vk::PipelineStageFlagBits::eColorAttachmentOutput |
         vk::PipelineStageFlagBits::eEarlyFragmentTests);
    dependency.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
    // destination
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        (vk::PipelineStageFlagBits::eColorAttachmentOutput |
         vk::PipelineStageFlagBits::eEarlyFragmentTests);
    dependency.dstAccessMask =
        (vk::AccessFlagBits::eColorAttachmentWrite |
         vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    // RenderPass ----------
    std::array<vk::AttachmentDescription, 3> attachments = {
        colorAttachment, depthAttachment, colorAttachmentResolve};
    vk::RenderPassCreateInfo renderPassCI{};
    renderPassCI.attachmentCount = attachments.size();
    renderPassCI.pAttachments = attachments.data();
    renderPassCI.subpassCount = 1;
    renderPassCI.pSubpasses = &subpass;
    renderPassCI.dependencyCount = 1;
    renderPassCI.pDependencies = &dependency;

    renderPass = renderEngine->getDevice().createRenderPass(renderPassCI);
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Default RenderPass has been created.";
}

void BasicRenderTarget::setupImageResources() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Creating default ImageResources...";
    // Color Image
    createImage(colorImageResource, imageExtent, 1,
                renderEngine->getEngineInfo().limit.maxMsaaSamples,
                colorImageFormat, vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransientAttachment |
                    vk::ImageUsageFlagBits::eColorAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                *renderEngine->getVmaAllocator());

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Color Image has been created.";

    createImageView(colorImageResource, colorImageFormat,
                    vk::ImageAspectFlagBits::eColor, 1,
                    renderEngine->getDevice());

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Color ImageView has been created.";

    // Depth Image
    createImage(depthImageResource, imageExtent, 1,
                renderEngine->getEngineInfo().limit.maxMsaaSamples,
                findDepthFormat(renderEngine), vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                *renderEngine->getVmaAllocator());

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Depth Image has been created.";

    createImageView(depthImageResource, findDepthFormat(renderEngine),
                    vk::ImageAspectFlagBits::eDepth, 1,
                    renderEngine->getDevice());

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default ImageResources has been created.";
}

void BasicRenderTarget::setupFrameBuffers() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Creating default FrameBuffers...";

    frameBuffers.resize(numOfColorImages);

    for (int i = 0; i < numOfColorImages; i++) {
        std::array<vk::ImageView, 3> attachments = {
            colorImageResource.view, depthImageResource.view,
            renderImageResources[i].view};

        vk::FramebufferCreateInfo frameBufferCI{};
        frameBufferCI.renderPass = renderPass;
        frameBufferCI.attachmentCount =
            static_cast<uint32_t>(attachments.size());
        frameBufferCI.pAttachments = attachments.data();
        frameBufferCI.width = imageExtent.width;
        frameBufferCI.height = imageExtent.height;
        frameBufferCI.layers = 1;

        vk::Result result = renderEngine->getDevice().createFramebuffer(
            &frameBufferCI, nullptr, &frameBuffers[i]);
        switch (result) {
        case vk::Result::eErrorOutOfHostMemory:
            throw std::runtime_error(
                "Failed to create frameBuffer: Out Of Host Memory.");
            break;
        case vk::Result::eErrorOutOfDeviceMemory:
            throw std::runtime_error(
                "Failed to create frameBuffer: Out Of Device Memory.");
            break;
        default:
            break;
        }
    }

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default FrameBuffers has been created.";
}

void BasicRenderTarget::setupGraphicsPipeline() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Creating default GraphicsPipeline...";

    // ShaderModules ----------
    //    auto vertShaderModule = createShaderModuleFromFile(
    //        createResourceDirectoryPath(std::filesystem::path("shaders") /
    //        "vert.spv"), renderEngine->getDevice());
    //    auto fragShaderModule = createShaderModuleFromFile(
    //        createResourceDirectoryPath(std::filesystem::path("shaders") /
    //        "frag.spv"), renderEngine->getDevice());

    auto vertShader = compileShader(ikura::VERTEX_SHADER_CODE, EShLangVertex);
    auto vertShaderModule =
        createShaderModule(vertShader, renderEngine->getDevice());

    auto fragShader =
        compileShader(ikura::FRAGMENT_SHADER_CODE, EShLangFragment);
    auto fragShaderModule =
        createShaderModule(fragShader, renderEngine->getDevice());

    vk::PipelineShaderStageCreateInfo vertShaderStageCI{};
    vertShaderStageCI.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageCI.module = vertShaderModule;
    vertShaderStageCI.pName = "main";

    vk::PipelineShaderStageCreateInfo fragShaderStageCI{};
    fragShaderStageCI.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageCI.module = fragShaderModule;
    fragShaderStageCI.pName = "main";

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {
        vertShaderStageCI, fragShaderStageCI};

    auto bindingDescription = BasicVertex::getBindingDescription();
    auto attributeDescriptions = BasicVertex::getAttributeDescriptions();

    // Pipeline input states ----------
    vk::PipelineVertexInputStateCreateInfo vertInputStateCI{};
    vertInputStateCI.vertexBindingDescriptionCount = 1;
    vertInputStateCI.pVertexBindingDescriptions = &bindingDescription;
    vertInputStateCI.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertInputStateCI.pVertexAttributeDescriptions =
        attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCI{};
    inputAssemblyCI.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemblyCI.primitiveRestartEnable = VK_FALSE;

    // Viewport state ----------
    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)imageExtent.width;
    viewport.height = (float)imageExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = imageExtent;

    vk::PipelineViewportStateCreateInfo viewportStateCI{};
    viewportStateCI.viewportCount = 1;
    viewportStateCI.pViewports = &viewport;
    viewportStateCI.scissorCount = 1;
    viewportStateCI.pScissors = &scissor;

    // Other states (render configrations) ----------
    vk::PipelineRasterizationStateCreateInfo rasterizerCI{};
    rasterizerCI.depthClampEnable = VK_FALSE;
    rasterizerCI.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCI.polygonMode = vk::PolygonMode::eFill;
    rasterizerCI.lineWidth = 1.0f;
    rasterizerCI.cullMode = vk::CullModeFlagBits::eBack;
    rasterizerCI.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizerCI.depthBiasEnable = VK_FALSE;

    vk::PipelineMultisampleStateCreateInfo multisamplingCI{};
    multisamplingCI.sampleShadingEnable = VK_FALSE;
    multisamplingCI.rasterizationSamples =
        renderEngine->getEngineInfo().limit.maxMsaaSamples;
    multisamplingCI.minSampleShading = 1.0f;
    multisamplingCI.pSampleMask = nullptr;

    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = vk::CompareOp::eLess;

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport, vk::DynamicState::eLineWidth};
    vk::PipelineDynamicStateCreateInfo dynamicStateCI{};
    dynamicStateCI.dynamicStateCount =
        static_cast<uint32_t>(dynamicStates.size());
    dynamicStateCI.pDynamicStates = dynamicStates.data();

    // Color blend ----------
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.colorWriteMask =
        vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB;
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCI{};
    colorBlendStateCI.logicOpEnable = VK_FALSE;
    colorBlendStateCI.logicOp = vk::LogicOp::eCopy;
    colorBlendStateCI.attachmentCount = 1;
    colorBlendStateCI.pAttachments = &colorBlendAttachmentState;
    colorBlendStateCI.blendConstants[0] = 0.0f;
    colorBlendStateCI.blendConstants[1] = 0.0f;
    colorBlendStateCI.blendConstants[2] = 0.0f;
    colorBlendStateCI.blendConstants[3] = 0.0f;

    // Pipeline layout ----------
    vk::PipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.setLayoutCount = 1;
    pipelineLayoutCI.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutCI.pushConstantRangeCount = 0;
    pipelineLayoutCI.pPushConstantRanges = nullptr;

    graphicsPipelineLayout =
        renderEngine->getDevice().createPipelineLayout(pipelineLayoutCI);

    // GraphicsPipeline ----------
    vk::GraphicsPipelineCreateInfo graphicsPipelineCI{};
    graphicsPipelineCI.stageCount = 2;
    graphicsPipelineCI.pStages = shaderStages.data();

    graphicsPipelineCI.pVertexInputState = &vertInputStateCI;
    graphicsPipelineCI.pInputAssemblyState = &inputAssemblyCI;
    graphicsPipelineCI.pViewportState = &viewportStateCI;
    graphicsPipelineCI.pRasterizationState = &rasterizerCI;
    graphicsPipelineCI.pMultisampleState = &multisamplingCI;
    graphicsPipelineCI.pDepthStencilState = nullptr;
    graphicsPipelineCI.pColorBlendState = &colorBlendStateCI;
    graphicsPipelineCI.pDynamicState = nullptr;
    graphicsPipelineCI.pDepthStencilState = &depthStencil;

    graphicsPipelineCI.layout = graphicsPipelineLayout;
    graphicsPipelineCI.renderPass = renderPass;
    graphicsPipelineCI.subpass = 0;
    graphicsPipelineCI.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCI.basePipelineIndex = -1;

    auto result = renderEngine->getDevice().createGraphicsPipeline(
        VK_NULL_HANDLE, graphicsPipelineCI);
    switch (result.result) {
    case vk::Result::eErrorOutOfHostMemory:
        throw std::runtime_error(
            "Failed to create GraphicsPipeline: Out Of Host Memory.");
        break;
    case vk::Result::eErrorOutOfDeviceMemory:
        throw std::runtime_error(
            "Failed to create GraphicsPipeline: Out Of Device Memory.");
        break;
    case vk::Result::eSuccess:
        break;
    default:
        throw std::runtime_error("Failed to create GraphicsPipeline.");
        break;
    }
    graphicsPipeline = result.value;

    renderEngine->getDevice().destroyShaderModule(vertShaderModule, nullptr);
    renderEngine->getDevice().destroyShaderModule(fragShaderModule, nullptr);

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default GraphicsPipeline has been created.";
}

BasicRenderTarget::BasicRenderTarget(
    const std::shared_ptr<RenderEngine> renderEngine,
    vk::Format colorImageFormat, vk::Extent2D imageExtent,
    vk::DescriptorSetLayout descriptorSetLayout,
    std::vector<vk::Image> &renderImages, int numOfFrames)

    : RenderTarget(renderEngine, colorImageFormat, imageExtent,
                   descriptorSetLayout, renderImages, numOfFrames) {

    setupRenderPass();
    setupImageResources();
    setupFrameBuffers();
    setupGraphicsPipeline();
}

void BasicRenderTarget::recreateResourcesForSwapChainRecreation(
    vk::Extent2D imageExtent, std::vector<vk::Image> renderImages) {

    this->imageExtent = imageExtent;
    this->numOfColorImages = renderImages.size();

    // init renderImageResources with renderImages
    renderImageResources.resize(numOfColorImages);
    for (int i = 0; i < numOfColorImages; i++) {
        renderImageResources[i].image = renderImages[i];
        RenderTarget::createImageView(renderImageResources[i], colorImageFormat,
                                      vk::ImageAspectFlagBits::eColor, 1,
                                      renderEngine->getDevice());
        renderImageResources[i].releaseImage = false;
    }

    setupImageResources();
    setupRenderPass();
    setupGraphicsPipeline();
    setupFrameBuffers();
}
} // namespace ikura