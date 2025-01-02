#pragma once

#include "../renderContent.hpp"

namespace ikura {
// Forward declearation
class Window;

class BasicRenderContent : public RenderContent {
  protected:
    std::vector<BasicVertex> vertices;
    std::vector<BasicIndex> indices;

    void setupUniformBuffers();
    void setupDescriptorSets();

  public:
    BasicRenderContent(std::shared_ptr<RenderEngine> renderEngine,
                       vk::DescriptorSetLayout descriptorSetLayout,
                       int numOfFrames);

    void addVertices(const std::vector<BasicVertex> &vertices);
    void addIndices(const std::vector<BasicIndex> &indices);
    void setVertices(const std::vector<BasicVertex> &vertices);
    void setIndices(const std::vector<BasicIndex> &indices);

    void updateUniformBuffer(int frameIndex, BasicModelMatUBO &modelMatUBO,
                             BasicSceneMatUBO &sceneMatUBO);

    // Implementation of virtual functions ----------
    void uploadVertexBuffer() override;
    void uploadIndexBuffer() override;
    const size_t getNumOfIndex() override;

    // Demo ----------
    void setDemoShape();
    void updateDemoUBO(std::shared_ptr<Window> window);
};
} // namespace ikura