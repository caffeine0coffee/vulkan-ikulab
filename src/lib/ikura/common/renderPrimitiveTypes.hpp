#pragma once

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace ikura {
namespace shapes {

struct Vertex {
    // TODO: decl virtual functions
};

struct BasicVertex : public Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    uint32_t id;

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    // TODO: array to vector
    static std::array<vk::VertexInputAttributeDescription, 3>
    getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 3>
            attributeDescriptions;
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof(BasicVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(BasicVertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32Uint;
        attributeDescriptions[2].offset = offsetof(BasicVertex, id);

        return attributeDescriptions;
    }

    bool operator==(const BasicVertex &other) const {
        return (pos == other.pos && color == other.color && id == other.id);
    }
};

typedef uint32_t Index;
// todo: delete this
typedef uint32_t JointID;
} // namespace shapes
} // namespace ikura