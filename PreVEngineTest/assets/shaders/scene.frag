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

const float ambient = 0.2;
const float specularExponent = 50.0;
const float specularDamper = 0.75;
const bool enablePCF = true;

float getShadow(const vec4 shadowCoord, const vec2 shadowCoordOffset)
{
	float shadow = 1.0;
	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
	{
		float depth = texture(depthSampler, shadowCoord.xy + shadowCoordOffset).r;
		if (shadowCoord.w > 0.0 && depth < shadowCoord.z) 
		{
			shadow = ambient;
		}
	}
	return shadow;
}

float getShadowPCF(const vec4 shadowCoord)
{
	const ivec2 texDim = textureSize(depthSampler, 0);
	const float scale = 1.5;
	const float dx = scale * 1.0 / float(texDim.x);
	const float dy = scale * 1.0 / float(texDim.y);
	const int range = 1; // 3 x 3

	float shadowFactor = 0.0;
	int count = 0;
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += getShadow(shadowCoord, vec2(dx * x, dy * y));
			count++;
		}
	
	}

	return shadowFactor / count;
}

void main() 
{
	float shadow = 1.0f;
	vec4 normalizedShadowCoord = inShadowCoord / inShadowCoord.w;
	if(enablePCF)
	{
		shadow = getShadowPCF(normalizedShadowCoord);
	}
	else
	{
		shadow = getShadow(normalizedShadowCoord, vec2(0.0));
	}

	const vec4 textureColor = texture(textureSampler, inTextureCoord);

	const vec3 N = normalize(inNormal);
	const vec3 L = normalize(inToLightDirectionVector);
	const vec3 V = normalize(inViewSpacePosition);
	const vec3 R = normalize(-reflect(L, N));
	const vec3 diffuse = max(dot(N, L), ambient) * textureColor.xyz;
	const vec3 specular = pow(max(dot(R, V), 0.0), specularExponent) * vec3(specularDamper);

	outColor = vec4((diffuse + specular) * shadow, 1.0);
	outColor.a = textureColor.a;
}