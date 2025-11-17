#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {
	vec4 fogColor;
    vec4 lowerLimit;
    vec4 upperLimit;
} uboFS;

layout(binding = 2) uniform samplerCube cubeMap1;

layout(location = 0) in vec3 inTextureCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    const vec3 tc = vec3(-inTextureCoord.x, inTextureCoord.y, inTextureCoord.z);
    vec4 finalColor = texture(cubeMap1, tc);

	float factor = (inTextureCoord.y - uboFS.lowerLimit.x) / (uboFS.upperLimit.x - uboFS.lowerLimit.x);
	factor = clamp(factor, 0.0, 1.0);

    outColor = mix(vec4(uboFS.fogColor.xyz, 1.0), finalColor, factor);
}