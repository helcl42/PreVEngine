#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConsts {
    uint cascadeIndex;
    float nearClippingPlane;
    float farClippingPlane;
} pushConsts;

layout (binding = 0) uniform sampler2DArray depthSampler;

layout (location = 0) in vec2 textureCoord;

layout (location = 0) out vec4 outFragColor;

float LinearizeDepth(const float near, const float far, const float depth)
{
  float n = near;
  float f = far;
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));
}

void main() 
{
	const float depth = texture(depthSampler, vec3(textureCoord, float(pushConsts.cascadeIndex))).r;
	outFragColor = vec4(vec3(1.0 - LinearizeDepth(pushConsts.nearClippingPlane, pushConsts.farClippingPlane, depth)), 1.0);
}