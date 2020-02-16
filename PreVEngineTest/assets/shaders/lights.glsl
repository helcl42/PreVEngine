// lightning client code

float getAttenuationFactor(in vec3 attenuation, in vec3 toLightVector) 
{
	float toLightDistance = length(toLightVector);
	float attenuationFactor = attenuation.x + (attenuation.y * toLightDistance) + (attenuation.z * toLightDistance * toLightDistance);
	return attenuationFactor;
}

vec3 getDiffuseLight(in vec3 normal, in vec3 toLightVector, in vec3 lightColor, in float attenuationFactor) 
{
	float nDotL = dot(normal, toLightVector);
	float brightness = max(nDotL, 0.0);
	return (brightness * lightColor) / attenuationFactor;
}

vec3 getSpecularLight(in vec3 normal, in vec3 toLightVector, in vec3 toCameraVector, in vec3 lightColor, in float attenuationFactor, in float shineDamper, in float reflectivity)
{
	vec3 lightDirection = -toLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection, normal);
	float specularFactor = dot(reflectedLightDirection, toCameraVector);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	return (dampedFactor * reflectivity * lightColor) / attenuationFactor;
}


// const vec3 unitNormal = normalize(inNormal);
// const vec3 unitToCameraVector = normalize(inToCameraVector);

// vec3 totalDiffuse = vec3(0.0);
// vec3 totalSpecular = vec3(0.0);
// for (int i = 0; i < uboFS.realCountOfLights; i++)
// {
// 	const float attenuationFactor = getAttenuationFactor(uboFS.attenuations[i].xyz, inToLightVectors[i]);
// 	const vec3 unitToLightVector = normalize(inToLightVectors[i]);
// 	totalDiffuse += getDiffuseLight(unitNormal, unitToLightVector, uboFS.lightColors[i].xyz, attenuationFactor);
// 	totalSpecular += getSpecularLight(unitNormal, unitToLightVector, unitToCameraVector, uboFS.lightColors[i].xyz, attenuationFactor, uboFS.shineDamper, uboFS.reflectivity);
// }
// totalDiffuse = max(totalDiffuse * shadow, 0.0) + uboFS.ambientLight;
// totalSpecular = totalSpecular * shadow;


// vec3 GetTotalDiffuseLight()
// {

// }