#include "renderContent.hpp"

#include <easylogging++.h>
#include <vulkan/vulkan.hpp>

#include "common/logLevels.hpp"
#include "window/nativeWindow/nativeWindow.hpp"

namespace ikura {
void BufferResource::release(VmaAllocator allocator) {
    if (buffer) {
        vmaDestroyBuffer(allocator, (VkBuffer)buffer, alloc);
    }
}

RenderContent::RenderContent(std::shared_ptr<RenderEngine> renderEngine,
                             vk::DescriptorSetLayout descriptorSetLayout,
                             int numOfFrames) {

    this->renderEngine = renderEngine;
    this->descriptorSetLayout = descriptorSetLayout;
    this->numOfFrames = numOfFrames;
}

RenderContent::~RenderContent() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying default DescriptorPool...";
    renderEngine->getDevice().destroyDescriptorPool(descriptorPool);
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default DescriptorPool has been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Destroying default UniformBuffers...";
    for (auto &resources : uniformBufferResources) {
        for (auto &r : resources) {
            r.release(*renderEngine->getVmaAllocator());
        }
    }
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default UniformBuffers has been destroyed.";

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Destroying VertexBuffer and IndexBuffer...";
    vertexBufferResource.release(*renderEngine->getVmaAllocator());
    indexBufferResource.release(*renderEngine->getVmaAllocator());
    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "VertexBuffer and IndexBuffer have been destroyed.";
}

void RenderContent::uploadVertexBuffer() {}

void RenderContent::uploadIndexBuffer() {}

const vk::Buffer &RenderContent::getVertexBuffer() const {
    return vertexBufferResource.buffer;
}

const vk::Buffer &RenderContent::getIndexBuffer() const {
    return indexBufferResource.buffer;
}

const std::vector<vk::DescriptorSet> &
RenderContent::getDescriptorSets(int index) {
    return descriptorSets[index];
}

const size_t RenderContent::getNumOfIndex() { return 0; }

void RenderContent::uploadViaStagingBuffer(
    void *srcData, BufferResource &dstBufferResource,
    vk::BufferUsageFlags dstBufferUsage, vk::DeviceSize bufferSize,
    std::shared_ptr<RenderEngine> renderEngine) {

    // Staging buffer allocation ----------
    BufferResource stagingBufferResource;
    vk::BufferCreateInfo stagingBufferCI{};
    stagingBufferCI.size = bufferSize;
    stagingBufferCI.usage = vk::BufferUsageFlagBits::eTransferSrc;

    VmaAllocationCreateInfo allocCI{};
    allocCI.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
    allocCI.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_MAPPED_BIT;
    VmaAllocationInfo stagingBufferAllocInfo{};

    VkBuffer vkBuffer;
    auto vkBufferCI = (VkBufferCreateInfo)stagingBufferCI;
    vmaCreateBuffer(*renderEngine->getVmaAllocator(), &vkBufferCI, &allocCI,
                    &vkBuffer, &stagingBufferResource.alloc,
                    &stagingBufferAllocInfo);
    stagingBufferResource.buffer = (vk::Buffer)vkBuffer;

    // Destination buffer allocation ----------
    vk::BufferCreateInfo dstBufferCI{};
    dstBufferCI.size = bufferSize;
    dstBufferCI.usage = dstBufferUsage | vk::BufferUsageFlagBits::eTransferDst;
    dstBufferCI.sharingMode = vk::SharingMode::eExclusive;

    allocCI = {};
    allocCI.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    vkBufferCI = (VkBufferCreateInfo)dstBufferCI;
    vmaCreateBuffer(*renderEngine->getVmaAllocator(), &vkBufferCI, &allocCI,
                    &vkBuffer, &dstBufferResource.alloc, nullptr);
    dstBufferResource.buffer = (vk::Buffer)vkBuffer;

    // Copy ----------
    // srcData to staging buffer
    memcpy(stagingBufferAllocInfo.pMappedData, srcData, bufferSize);

    // staging buffer to destination buffer
    auto cmdBuffer = renderEngine->beginSingleTimeCommands();

    vk::BufferCopy copy;
    copy.size = bufferSize;
    cmdBuffer.copyBuffer(stagingBufferResource.buffer, dstBufferResource.buffer,
                         copy);

    // submit command
    renderEngine->endSingleTimeCommands(cmdBuffer);

    stagingBufferResource.release(*renderEngine->getVmaAllocator());
}
} // namespace ikura