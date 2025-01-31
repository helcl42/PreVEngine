// lightning client code

const uint MAX_LIGHT_COUNT = 4;
const bool USE_BLINN = true;

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
	vec4 color;

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

	float specularFactor;
	float shinePower;
	if(USE_BLINN)
	{
		vec3 halfwayDir = normalize(toLightVector + toCameraVector);  
		specularFactor = max(dot(normal, halfwayDir), 0.0);	
		shinePower = shineDamper * 2.4; // TODO - some magic scale 
	}
	else 
	{
		specularFactor = max(dot(reflectedLightDirection, toCameraVector), 0.0);
		shinePower = shineDamper;
	}
	
	float dampedFactor = pow(specularFactor, shinePower);
	return (dampedFactor * reflectivity * lightColor) / attenuationFactor;
}