#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {
    vec4 color;
    float width;
    float edge;
    float borderWidth;
    float borderEdge;
    uint hasEffect;
    vec4 outlineColor;
    vec4 outlineOffset;
} uboFS;

layout(binding = 2) uniform sampler2D textureSampler;

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	float distance = 1.0 - texture(textureSampler, vec2(inTextureCoord.x, inTextureCoord.y)).a;
	float alpha = 1.0 - smoothstep(uboFS.width, uboFS.width + uboFS.edge, distance);

	if (uboFS.hasEffect != 0)
	{
		float distance2 = 1.0 - texture(textureSampler, vec2(inTextureCoord.x, inTextureCoord.y) + uboFS.outlineOffset.xy).a;
		float outlineAlpha = 1.0 - smoothstep(uboFS.borderWidth, uboFS.borderWidth + uboFS.borderEdge, distance2);

		float overallAlpha = alpha + (1.0 - alpha) * outlineAlpha;
		vec3 overallColor = mix(uboFS.outlineColor.xyz, uboFS.color.rgb, alpha / overallAlpha);

		outColor = vec4(overallColor, overallAlpha * uboFS.color.a);
	}
	else
	{
		outColor = vec4(uboFS.color.rgb, alpha * uboFS.color.a);
	}
}