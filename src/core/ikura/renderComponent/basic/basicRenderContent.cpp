#include "basicRenderContent.hpp"

#include <easylogging++.h>

#include "common/logLevels.hpp"
#include "descriptorSetProps.hpp"
#include "shape/shapes.hpp"
#include "window/window.hpp"

// for demo
#include <glm/gtc/matrix_transform.hpp>

namespace ikura {
void BasicRenderContent::setupUniformBuffers() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Creating default UniformBuffers...";

    std::array<vk::DeviceSize, NUM_OF_DESCRIPTORS> uniformBufferSizes;
    uniformBufferSizes[DESCRIPTOR_SET_BINDING_MODEL_MATRIX_UBO] =
        sizeof(BasicModelMatUBO);
    uniformBufferSizes[DESCRIPTOR_SET_BINDING_SCENE_MATRIX_UBO] =
        sizeof(BasicSceneMatUBO);

    uniformBufferResources.resize(numOfFrames);

    for (size_t frame = 0; frame < numOfFrames; frame++) {
        uniformBufferResources[frame].resize(NUM_OF_DESCRIPTORS);

        for (size_t descSetIndex = 0; descSetIndex < NUM_OF_DESCRIPTORS;
             descSetIndex++) {
            // TODO: create large DeviceMemory and assign part of them to each
            // uniformBuffers
            vk::BufferCreateInfo bufferCI{};
            bufferCI.size = uniformBufferSizes[descSetIndex];
            bufferCI.usage = vk::BufferUsageFlagBits::eUniformBuffer;
            bufferCI.sharingMode = vk::SharingMode::eExclusive;

            VmaAllocationCreateInfo allocCI{};
            allocCI.usage = VMA_MEMORY_USAGE_AUTO;
            allocCI.flags =
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT;

            auto vkBufferCI = (VkBufferCreateInfo)bufferCI;
            VkBuffer vkBuffer;

            vmaCreateBuffer(*renderEngine->getVmaAllocator(), &vkBufferCI,
                            &allocCI, &vkBuffer,
                            &uniformBufferResources[frame][descSetIndex].alloc,
                            nullptr);

            vk::Buffer buffer(vkBuffer);
            uniformBufferResources[frame][descSetIndex].buffer = buffer;
        }
    }

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default UniformBuffers has been created.";
}

void BasicRenderContent::setupDescriptorSets() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Creating default DescriptorSets...";

    // DescriptorPool ----------
    std::array<vk::DescriptorPoolSize, 2> poolSizes;
    poolSizes[DESCRIPTOR_SET_BINDING_MODEL_MATRIX_UBO].type =
        vk::DescriptorType::eUniformBuffer;
    poolSizes[DESCRIPTOR_SET_BINDING_MODEL_MATRIX_UBO].descriptorCount =
        static_cast<uint32_t>(numOfFrames * NUM_OF_DESCRIPTORS);
    poolSizes[DESCRIPTOR_SET_BINDING_SCENE_MATRIX_UBO].type =
        vk::DescriptorType::eUniformBuffer;
    poolSizes[DESCRIPTOR_SET_BINDING_SCENE_MATRIX_UBO].descriptorCount =
        static_cast<uint32_t>(numOfFrames);

    vk::DescriptorPoolCreateInfo poolCI{};
    poolCI.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolCI.pPoolSizes = poolSizes.data();
    poolCI.maxSets =
        static_cast<uint32_t>(numOfFrames * NUM_OF_DESCRIPTOR_SETS);

    descriptorPool = renderEngine->getDevice().createDescriptorPool(poolCI);

    // DescriptorSets ==========
    // Array preparation ----------
    size_t numOfDescriptorWrites = numOfFrames * NUM_OF_DESCRIPTORS;

    std::vector<vk::WriteDescriptorSet> descriptorWrites;
    descriptorWrites.resize(numOfDescriptorWrites);
    std::vector<vk::DescriptorBufferInfo> bufferInfos;
    bufferInfos.resize(numOfDescriptorWrites);

    descriptorSets.resize(numOfFrames);

    // DescriptorSet Allocation loop per frame ----------
    size_t index = 0;
    for (size_t frame = 0; frame < numOfFrames; frame++) {
        // Allocation ----------
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = NUM_OF_DESCRIPTOR_SETS;
        allocInfo.pSetLayouts = &descriptorSetLayout;

        descriptorSets[frame] =
            renderEngine->getDevice().allocateDescriptorSets(allocInfo);

        // Fill Update Info ----------
        // Model Matrix UBO
        bufferInfos[index].buffer =
            uniformBufferResources[frame]
                                  [DESCRIPTOR_SET_BINDING_MODEL_MATRIX_UBO]
                                      .buffer;
        bufferInfos[index].offset = 0;
        bufferInfos[index].range = VK_WHOLE_SIZE;

        descriptorWrites[index].dstSet =
            descriptorSets[frame][DESCRIPTOR_SET_INDEX_MODEL_MATRIX_UBO];
        descriptorWrites[index].dstBinding =
            DESCRIPTOR_SET_BINDING_MODEL_MATRIX_UBO;
        descriptorWrites[index].dstArrayElement = 0;
        descriptorWrites[index].descriptorType =
            vk::DescriptorType::eUniformBuffer;
        descriptorWrites[index].descriptorCount = 1;
        descriptorWrites[index].pBufferInfo = &bufferInfos[index];
        index++;

        // Scene Matrix UBO
        bufferInfos[index].buffer =
            uniformBufferResources[frame]
                                  [DESCRIPTOR_SET_BINDING_SCENE_MATRIX_UBO]
                                      .buffer;
        bufferInfos[index].offset = 0;
        bufferInfos[index].range = VK_WHOLE_SIZE;

        descriptorWrites[index].dstSet =
            descriptorSets[frame][DESCRIPTOR_SET_INDEX_SCENE_MATRIX_UBO];
        descriptorWrites[index].dstBinding =
            DESCRIPTOR_SET_BINDING_SCENE_MATRIX_UBO;
        descriptorWrites[index].dstArrayElement = 0;
        descriptorWrites[index].descriptorType =
            vk::DescriptorType::eUniformBuffer;
        descriptorWrites[index].descriptorCount = 1;
        descriptorWrites[index].pBufferInfo = &bufferInfos[index];
        index++;
    }

    // Update ----------
    renderEngine->getDevice().updateDescriptorSets(
        index, descriptorWrites.data(), 0, nullptr);

    VLOG(VLOG_LV_3_PROCESS_TRACKING)
        << "Default DescriptorSets has been created.";
}

BasicRenderContent::BasicRenderContent(
    std::shared_ptr<RenderEngine> renderEngine,
    vk::DescriptorSetLayout descriptorSetLayout, int numOfFrames)

    : RenderContent(renderEngine, descriptorSetLayout, numOfFrames) {

    setupUniformBuffers();
    setupDescriptorSets();
}

void BasicRenderContent::addVertices(const std::vector<BasicVertex> &vertices) {
    this->vertices.insert(this->vertices.end(), vertices.begin(),
                          vertices.end());
}

void BasicRenderContent::addIndices(const std::vector<BasicIndex> &indices) {
    this->indices.insert(this->indices.end(), indices.begin(), indices.end());
}

void BasicRenderContent::setVertices(const std::vector<BasicVertex> &vertices) {
    this->vertices = vertices;
}

void BasicRenderContent::setIndices(const std::vector<BasicIndex> &indices) {
    this->indices = indices;
}

void BasicRenderContent::updateUniformBuffer(int frameIndex,
                                             BasicModelMatUBO &modelMatUBO,
                                             BasicSceneMatUBO &sceneMatUBO) {
    void *data;
    // Model Matrix
    vmaMapMemory(*renderEngine->getVmaAllocator(),
                 uniformBufferResources[frameIndex]
                                       [DESCRIPTOR_SET_BINDING_MODEL_MATRIX_UBO]
                                           .alloc,
                 &data);
    memcpy(data, &modelMatUBO, sizeof(modelMatUBO));
    vmaUnmapMemory(
        *renderEngine->getVmaAllocator(),
        uniformBufferResources[frameIndex]
                              [DESCRIPTOR_SET_BINDING_MODEL_MATRIX_UBO]
                                  .alloc);

    // Scene Matrix
    vmaMapMemory(*renderEngine->getVmaAllocator(),
                 uniformBufferResources[frameIndex]
                                       [DESCRIPTOR_SET_BINDING_SCENE_MATRIX_UBO]
                                           .alloc,
                 &data);
    memcpy(data, &sceneMatUBO, sizeof(sceneMatUBO));
    vmaUnmapMemory(
        *renderEngine->getVmaAllocator(),
        uniformBufferResources[frameIndex]
                              [DESCRIPTOR_SET_BINDING_SCENE_MATRIX_UBO]
                                  .alloc);
}

void BasicRenderContent::uploadIndexBuffer() {
    if (indices.empty()) {
        LOG(INFO) << "Index array is empty. Stopping indexBuffer upload.";
        return;
    }

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Uploading IndexBuffer...";

    indexBufferResource.release(*renderEngine->getVmaAllocator());

    uploadViaStagingBuffer(indices.data(), indexBufferResource,
                           vk::BufferUsageFlagBits::eIndexBuffer,
                           sizeof(indices[0]) * indices.size(), renderEngine);

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "IndexBuffer has been uploaded.";
}

void BasicRenderContent::uploadVertexBuffer() {
    if (vertices.empty()) {
        LOG(INFO) << "Vertex array is empty. Stopping vertexBuffer upload.";
        return;
    }

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Uploading VertexBuffer...";

    vertexBufferResource.release(*renderEngine->getVmaAllocator());

    uploadViaStagingBuffer(
        BasicVertex::convertToDataVector(vertices).data(), vertexBufferResource,
        vk::BufferUsageFlagBits::eVertexBuffer,
        sizeof(BasicVertex::Data) * vertices.size(), renderEngine);

    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "VertexBuffer has been uploaded.";
}

const size_t BasicRenderContent::getNumOfIndex() { return indices.size(); }

// Demo ----------

void BasicRenderContent::setDemoShape() {
    shapes::SingleColorCube cube(1.0, 1.0, 1.0, {0, 0, 0}, {1.0, 1.0, 1.0}, 0);

    setVertices(cube.getVertices());
    setIndices(cube.getIndices());
}

void BasicRenderContent::updateDemoUBO(std::shared_ptr<Window> window) {
    BasicModelMatUBO modelMat;
    modelMat.model[0] = glm::mat4(1.0);

    BasicSceneMatUBO sceneMat;
    sceneMat.view =
        glm::lookAt({2.0, 2.0, 4.0} /* eye */, {0.0, 0.0, 0.0} /* center */,
                    glm::vec3(0.0, 0.0, 1.0) /* up */);
    sceneMat.proj = glm::perspective(
        glm::radians(45.0f), window->getWidth() / (float)window->getHeight(),
        0.01f, 1000.0f);
    // Convert to RightHand Z-up
    sceneMat.proj[1][1] *= -1;

    // update all frame
    for (int i = 0; i < window->getNumOfFrames(); i++) {
        updateUniformBuffer(i, modelMat, sceneMat);
    }
}
} // namespace ikura