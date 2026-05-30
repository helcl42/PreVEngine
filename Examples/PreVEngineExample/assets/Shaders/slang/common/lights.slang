// Lighting module
module lights;

public static const uint MAX_LIGHT_COUNT = 4;
public static const bool USE_BLINN = true;

public struct Light
{
    public float4 position;
    public float4 color;
    public float4 attenuation;
};

public struct Lightning
{
    public Light lights[MAX_LIGHT_COUNT];
    public uint realCountOfLights;
    public float ambientFactor;
};

public struct Material
{
    public float4 color;
    public float shineDamper;
    public float reflectivity;
};

public float GetAttenuationFactor(float3 attenuation, float3 toLightVector)
{
    float toLightDistance = length(toLightVector);
    float attenuationFactor = attenuation.x + (attenuation.y * toLightDistance) + (attenuation.z * toLightDistance * toLightDistance);
    return attenuationFactor;
}

public float3 GetDiffuseColor(float3 normal, float3 toLightVector, float3 lightColor, float attenuationFactor)
{
    float nDotL = dot(normal, toLightVector);
    float brightness = max(nDotL, 0.0);
    return (brightness * lightColor) / attenuationFactor;
}

public float3 GetSpecularColor(float3 normal, float3 toLightVector, float3 toCameraVector, float3 lightColor, float attenuationFactor, float shineDamper, float reflectivity)
{
    float3 lightDirection = -toLightVector;
    float3 reflectedLightDirection = reflect(lightDirection, normal);

    float specularFactor;
    float shinePower;
    if (USE_BLINN)
    {
        float3 halfwayDir = normalize(toLightVector + toCameraVector);
        specularFactor = max(dot(normal, halfwayDir), 0.0);
        shinePower = shineDamper * 2.4;
    }
    else
    {
        specularFactor = max(dot(reflectedLightDirection, toCameraVector), 0.0);
        shinePower = shineDamper;
    }

    float dampedFactor = pow(specularFactor, shinePower);
    return (dampedFactor * reflectivity * lightColor) / attenuationFactor;
}
