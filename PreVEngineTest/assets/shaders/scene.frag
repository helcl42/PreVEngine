#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D textureSampler;
layout(binding = 2) uniform sampler2D depthSampler;

layout(location = 0) in vec2 inTextureCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inToLightDirectionVector;
layout(location = 3) in vec3 inViewSpacePosition;
layout(location = 4) in vec4 inShadowCoord;

layout(location = 0) out vec4 outColor;

#define ambient 0.1

float getShadow(const vec4 shadowCoord, const vec2 offset)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float depth = texture(depthSampler, shadowCoord.st + offset).r;
		if (shadowCoord.w > 0.0 && depth < shadowCoord.z) 
		{
			shadow = ambient;
		}
	}
	return shadow;
}

void main() {
	float shadow = getShadow(inShadowCoord / inShadowCoord.w, vec2(0.0));
	vec4 textureColor = texture(textureSampler, inTextureCoord);

	vec3 N = normalize(inNormal);
	vec3 L = normalize(inToLightDirectionVector);
	vec3 V = normalize(inViewSpacePosition);
	vec3 R = normalize(-reflect(L, N));
	vec3 diffuse = max(dot(N, L), ambient) * textureColor.xyz;

	outColor = vec4(diffuse * shadow, 1.0);
	outColor.a = textureColor.a;
}