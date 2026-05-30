// Animated textureless shader
import common.utils;
import common.common;
import common.lights;
import common.shadows;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

static const int MAX_BONES_COUNT = 100;

struct AnimationVSParams
{
    float4x4 bones[MAX_BONES_COUNT];
    float4x4 modelMatrix;
    float4x4 normalMatrix;
    float4x4 viewMatrices[MAX_VIEW_COUNT];
    float4x4 projectionMatrices[MAX_VIEW_COUNT];
    float4 cameraPositions[MAX_VIEW_COUNT];
    float4 clipPlane;
    Lightning lightning;
    float4 textureOffset;
    uint textureNumberOfRows;
    float density;
    float gradient;
};

struct AnimationTexturelessFSParams
{
    Shadows shadows;
    Lightning lightning;
    Material material;
    float4 fogColor;
    float4 selectedColor;
    uint selected;
    uint castedByShadows;
};

[[vk::binding(0)]] ConstantBuffer<AnimationVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<AnimationTexturelessFSParams> uboFS;
[[vk::binding(2)]] Texture2DArray depthTexture;
[[vk::binding(3)]] SamplerState depthSampler;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
    [[vk::location(3)]] int4 boneIds : BLENDINDICES;
    [[vk::location(4)]] float4 weights : BLENDWEIGHT;
};

struct Interpolants
{
    float4 position : SV_Position;
    [[vk::location(0)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float3 worldPosition : TEXCOORD1;
    [[vk::location(3)]] float3 viewPosition : TEXCOORD2;
    [[vk::location(4)]] float3 toCameraVector : TEXCOORD3;
    [[vk::location(5)]] float visibility : TEXCOORD4;
    [[vk::location(6)]] float3 toLightVector0 : TEXCOORD5;
    [[vk::location(7)]] float3 toLightVector1 : TEXCOORD6;
    [[vk::location(8)]] float3 toLightVector2 : TEXCOORD7;
    [[vk::location(9)]] float3 toLightVector3 : TEXCOORD8;
    [[vk::location(10)]] float clipDistance : TEXCOORD9;
};

[shader("vertex")]
Interpolants vertexMain(VertexInput input
#ifdef ENABLE_XR
    , uint viewIndex : SV_ViewID
#endif
)
{
#ifndef ENABLE_XR
    const int viewIndex = 0;
#endif
    Interpolants output;

    float4x4 boneTransform = uboVS.bones[input.boneIds[0]] * input.weights[0];
    boneTransform += uboVS.bones[input.boneIds[1]] * input.weights[1];
    boneTransform += uboVS.bones[input.boneIds[2]] * input.weights[2];
    boneTransform += uboVS.bones[input.boneIds[3]] * input.weights[3];

    float4 positionL = mul(boneTransform, float4(input.position, 1.0));
    float4 normalL = mul(boneTransform, float4(input.normal, 0.0));

    float4 worldPosition = mul(uboVS.modelMatrix, float4(positionL.xyz, 1.0));
    output.worldPosition = worldPosition.xyz;
    output.clipDistance = dot(worldPosition, uboVS.clipPlane);

    float4 viewPosition = mul(uboVS.viewMatrices[viewIndex], worldPosition);
    output.viewPosition = viewPosition.xyz;
    output.position = mul(uboVS.projectionMatrices[viewIndex], viewPosition);

    output.textureCoord = (input.textureCoord / uboVS.textureNumberOfRows) + uboVS.textureOffset.xy;
    output.normal = mul(uboVS.normalMatrix, float4(normalL.xyz, 0.0)).xyz;

    float3 toLightVectors[4];
    for (int i = 0; i < (int)uboVS.lightning.realCountOfLights; i++)
    {
        toLightVectors[i] = uboVS.lightning.lights[i].position.xyz - worldPosition.xyz;
    }

    output.toCameraVector = uboVS.cameraPositions[viewIndex].xyz - worldPosition.xyz;
    output.visibility = GetVisibility(viewPosition.xyz, uboVS.gradient, uboVS.density);

    output.toLightVector0 = toLightVectors[0];
    output.toLightVector1 = toLightVectors[1];
    output.toLightVector2 = toLightVectors[2];
    output.toLightVector3 = toLightVectors[3];
    return output;
}

[shader("fragment")]
float4 fragmentMain(Interpolants input) : SV_Target
{
    float3 toLightVectors[4] = { input.toLightVector0, input.toLightVector1, input.toLightVector2, input.toLightVector3 };
    if (input.clipDistance < 0.0)
    {
        discard;
    }

    float shadow = 1.0;
    if (uboFS.castedByShadows != 0)
    {
        shadow = GetShadow(depthTexture, depthSampler, uboFS.shadows, input.viewPosition, input.worldPosition, 0.005, input.position);
    }

    float3 unitNormal = normalize(input.normal);
    float3 unitToCameraVector = normalize(input.toCameraVector);

    float3 totalDiffuse = float3(0.0);
    float3 totalSpecular = float3(0.0);
    for (uint i = 0; i < uboFS.lightning.realCountOfLights; i++)
    {
        Light light = uboFS.lightning.lights[i];
        float3 toLightVector = toLightVectors[i];
        float3 unitToLightVector = normalize(toLightVector);
        float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
        totalDiffuse += GetDiffuseColor(unitNormal, unitToLightVector, light.color.xyz, attenuationFactor);
        totalSpecular += GetSpecularColor(unitNormal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, uboFS.material.shineDamper, uboFS.material.reflectivity);
    }
    totalDiffuse = totalDiffuse * shadow + uboFS.lightning.ambientFactor;
    totalSpecular = totalSpecular * shadow;

    float4 baseResultColor = float4(totalDiffuse, 1.0) * uboFS.material.color + float4(totalSpecular, 0.0);
    float4 resultColor = lerp(float4(uboFS.fogColor.xyz, 1.0), baseResultColor, input.visibility);

    if (uboFS.selected != 0)
    {
        resultColor = lerp(resultColor, uboFS.selectedColor, 0.5);
    }

    return resultColor;
}
