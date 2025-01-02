#pragma once

#include <string>

namespace ikura {
static const std::string VERTEX_SHADER_CODE = R"(
#version 450

const int NUM_OF_ID = 256;

layout(set = 0, binding = 0) uniform ModelMat {
	mat4 model[NUM_OF_ID];
} modelMat;

layout(set = 0, binding = 1) uniform SceneMat {
	mat4 view;
	mat4 proj;
} sceneMat;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in uint inId;

layout(location = 0) out vec3 flagColor;

void main() {
	gl_Position = sceneMat.proj * sceneMat.view * modelMat.model[inId] * vec4(inPosition, 1.0);

	flagColor = inColor;
}
)";

static const std::string FRAGMENT_SHADER_CODE = R"(
#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(fragColor, 1.0);
}
)";

}