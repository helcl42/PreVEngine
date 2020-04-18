#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {
    vec4 color;
    vec4 width;
    vec4 edge;
    uint hasEffect;
    vec4 borderWidth;
    vec4 borderEdge;
    vec4 outlineColor;
    vec4 outlineOffset;
} uboFS;

layout(binding = 2) uniform sampler2D textureSampler;

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	float distance = 1.0 - texture(textureSampler, vec2(inTextureCoord.x, inTextureCoord.y)).a;
	float alpha = 1.0 - smoothstep(uboFS.width.x, uboFS.width.x + uboFS.edge.x, distance);

	if (uboFS.hasEffect != 0)
	{
		float distance2 = 1.0 - texture(textureSampler, vec2(inTextureCoord.x, inTextureCoord.y) + uboFS.outlineOffset.xy).a;
		float outlineAlpha = 1.0 - smoothstep(uboFS.borderWidth.x, uboFS.borderWidth.x + uboFS.borderEdge.x, distance2);

		float overallAlpha = alpha + (1.0 - alpha) * outlineAlpha;
		vec3 overallColor = mix(uboFS.outlineColor.xyz, uboFS.color.rgb, alpha / overallAlpha);

		outColor = vec4(overallColor, overallAlpha * uboFS.color.a);
	}
	else
	{
		outColor = vec4(uboFS.color.rgb, alpha * uboFS.color.a);
	}
}