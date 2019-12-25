#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 normalMatrix;
    mat4 lightViewProjectionMatrix;
    vec3 lightPosition;
} ubo;

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTextureCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outToLightDirectionVector;
layout(location = 3) out vec3 outViewSpacePosition;
layout(location = 4) out vec4 outShadowCoord;

void main() {
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(inPosition, 1.0);

	outTextureCoord = inTextureCoord;
	outNormal = (ubo.normalMatrix * vec4(inNormal, 0.0)).xyz;
    outToLightDirectionVector = normalize(ubo.lightPosition - inPosition);
	outViewSpacePosition = -(ubo.modelMatrix * vec4(inPosition, 1.0)).xyz;
	outShadowCoord = (biasMat * ubo.lightViewProjectionMatrix * ubo.modelMatrix) * vec4(inPosition, 1.0);
}
