// lightning client code

const uint MAX_LIGHT_COUNT = 4;

struct Light {
	vec4 position;

	vec4 color;

	vec4 attenuation;
};

struct Lightning {
	Light lights[MAX_LIGHT_COUNT];

	uint realCountOfLights;

	float ambientFactor;
};

struct Material {
	float shineDamper;

	float reflectivity;
};

float GetAttenuationFactor(in vec3 attenuation, in vec3 toLightVector) 
{
	float toLightDistance = length(toLightVector);
	float attenuationFactor = attenuation.x + (attenuation.y * toLightDistance) + (attenuation.z * toLightDistance * toLightDistance);
	return attenuationFactor;
}

vec3 GetDiffuseColor(in vec3 normal, in vec3 toLightVector, in vec3 lightColor, in float attenuationFactor) 
{
	float nDotL = dot(normal, toLightVector);
	float brightness = max(nDotL, 0.0);
	return (brightness * lightColor) / attenuationFactor;
}

vec3 GetSpecularColor(in vec3 normal, in vec3 toLightVector, in vec3 toCameraVector, in vec3 lightColor, in float attenuationFactor, in float shineDamper, in float reflectivity)
{
	vec3 lightDirection = -toLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection, normal);
	float specularFactor = dot(reflectedLightDirection, toCameraVector);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	return (dampedFactor * reflectivity * lightColor) / attenuationFactor;
}

vec3 GetDiffuseColor(in Lightning lightning, in float shadowFactor, in vec3 unitNormal, in vec3 unitToCameraVector, in vec3 worldPosition)
{
	vec3 totalDiffuse = vec3(0.0);
	for (uint i = 0; i < lightning.realCountOfLights; i++)
	{
		const Light light = lightning.lights[i];

		const vec3 toLightVector = light.position.xyz - worldPosition;

		const float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
		
		const vec3 unitToLightVector = normalize(toLightVector);
		totalDiffuse += GetDiffuseColor(unitNormal, unitToLightVector, light.color.xyz, attenuationFactor);
	}

	totalDiffuse = max(totalDiffuse * shadowFactor, 0.0) + lightning.ambientFactor;

	return totalDiffuse;
}

vec3 GetSpecularColor(in Lightning lightning, in Material material, in float shadowFactor, in vec3 unitNormal, in vec3 unitToCameraVector, in vec3 worldPosition)
{
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < lightning.realCountOfLights; i++)
	{
		const Light light = lightning.lights[i];

		const vec3 toLightVector = light.position.xyz - worldPosition.xyz;

		const float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
		
		const vec3 unitToLightVector = normalize(toLightVector);

		totalSpecular += GetSpecularColor(unitNormal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, material.shineDamper, material.reflectivity);
	}

	totalSpecular = totalSpecular * shadowFactor;

	return totalSpecular;
}

// optimized light calculation
void GetLightColor(in Lightning lightning, in Material material, in float shadowFactor, in vec3 unitNormal, in vec3 unitToCameraVector, in vec3 worldPosition, out vec3 outDiffuseColor, out vec3 outSpecularColor)
{
	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	for (uint i = 0; i < lightning.realCountOfLights; i++)
	{
		const Light light = lightning.lights[i];

		const vec3 toLightVector = light.position.xyz - worldPosition.xyz;

		const float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
		
		const vec3 unitToLightVector = normalize(toLightVector);

		totalSpecular += GetSpecularColor(unitNormal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, material.shineDamper, material.reflectivity);
		totalDiffuse += GetDiffuseColor(unitNormal, unitToLightVector, light.color.xyz, attenuationFactor);
	}

	totalDiffuse = max(totalDiffuse * shadowFactor, 0.0) + lightning.ambientFactor;	
	totalSpecular = totalSpecular * shadowFactor;

	outDiffuseColor = totalDiffuse;
	outSpecularColor = totalSpecular;
}