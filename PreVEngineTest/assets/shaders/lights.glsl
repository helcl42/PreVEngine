// lightning client code

float getAttenuationFactor(const vec3 attenuation, const vec3 toLightVector) 
{
	float toLightDistance = length(toLightVector);
	float attenuationFactor = attenuation.x + (attenuation.y * toLightDistance) + (attenuation.z * toLightDistance * toLightDistance);
	return attenuationFactor;
}

vec3 getDiffuseLight(const vec3 normal, const vec3 toLightVector, const vec3 lightColor, const float attenuationFactor) 
{
	float nDotL = dot(normal, toLightVector);
	float brightness = max(nDotL, 0.0);
	return (brightness * lightColor) / attenuationFactor;
}

vec3 getSpecularLight(const vec3 normal, const vec3 toLightVector, const vec3 toCameraVector, const vec3 lightColor, const float attenuationFactor, const float shineDamper, const float reflectivity)
{
	vec3 lightDirection = -toLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection, normal);
	float specularFactor = dot(reflectedLightDirection, toCameraVector);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	return (dampedFactor * reflectivity * lightColor) / attenuationFactor;
}
