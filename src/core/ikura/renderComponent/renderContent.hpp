#pragma once

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

#include "../common/renderPrimitiveTypes.hpp"
#include "../common/uniformBufferInfo.hpp"
#include "../engine/renderEngine/renderEngine.hpp"
#include "./renderComponentProvider.hpp"

namespace ikura {
class BufferResource {
  public:
    vk::Buffer buffer;
    VmaAllocation alloc;

    void release(VmaAllocator allocator);
};

class RenderContent {
  protected:
    RenderContent(std::shared_ptr<RenderEngine> renderEngine,
                  vk::DescriptorSetLayout descriptorSetLayout, int numOfFrames);

    // Variables ==========
    std::shared_ptr<RenderEngine> renderEngine;

    // Buffers ----------
    BufferResource vertexBufferResource;
    BufferResource indexBufferResource;
    // uniformBuffers[frame][set]
    std::vector<std::vector<BufferResource>> uniformBufferResources;

    // about DescriptorSet ----------
    vk::DescriptorPool descriptorPool;
    // descriptorSets[frame][set]
    std::vector<std::vector<vk::DescriptorSet>> descriptorSets;
    vk::DescriptorSetLayout descriptorSetLayout;

    // Properties ----------
    int numOfFrames;

    // Functions ==========
    static void
    uploadViaStagingBuffer(void *srcData, BufferResource &dstBufferResource,
                           vk::BufferUsageFlags dstBufferUsage,
                           vk::DeviceSize bufferSize,
                           std::shared_ptr<RenderEngine> renderEngine);

  public:
    virtual ~RenderContent();

    // Upload to GPU ----------
    virtual void uploadVertexBuffer();
    virtual void uploadIndexBuffer();

    // Getter ----------
    virtual const size_t getNumOfIndex();
    const vk::Buffer &getVertexBuffer() const;
    const vk::Buffer &getIndexBuffer() const;
    const std::vector<vk::DescriptorSet> &getDescriptorSets(int index);
};
} // namespace ikura