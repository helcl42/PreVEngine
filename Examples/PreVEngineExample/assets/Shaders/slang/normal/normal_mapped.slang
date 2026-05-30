// Normal-mapped shader with tangent-space lighting
import common.utils;
import common.common;
import common.lights;
import common.shadows;
import common.normal_mapping;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct NormalMappedVSParams
{
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

struct NormalMappedFSParams
{
    Shadows shadows;
    Lightning lightning;
    Material material;
    float4 fogColor;
    float4 selectedColor;
    uint selected;
    uint castedByShadows;
    uint hasNormalMap;
};

[[vk::binding(0)]] ConstantBuffer<NormalMappedVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<NormalMappedFSParams> uboFS;
[[vk::binding(2)]] Texture2D colorTexture;
[[vk::binding(3)]] SamplerState colorSampler;
[[vk::binding(4)]] Texture2D normalTexture;
[[vk::binding(5)]] SamplerState normalSampler;
[[vk::binding(6)]] Texture2DArray depthTexture;
[[vk::binding(7)]] SamplerState depthSampler;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
    [[vk::location(3)]] float3 tangent : TANGENT;
    [[vk::location(4)]] float3 biTangent : BINORMAL;
};

struct Interpolants
{
    float4 position : SV_Position;
    [[vk::location(0)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float3 worldPosition : TEXCOORD1;
    [[vk::location(3)]] float3 viewPosition : TEXCOORD2;
    [[vk::location(4)]] float visibility : TEXCOORD3;
    [[vk::location(5)]] float3 toCameraVectorTangentSpace : TEXCOORD4;
    [[vk::location(6)]] float3 positionTangentSpace : TEXCOORD5;
    [[vk::location(7)]] float3 toLightVectorTangentSpace0 : TEXCOORD6;
    [[vk::location(8)]] float3 toLightVectorTangentSpace1 : TEXCOORD7;
    [[vk::location(9)]] float3 toLightVectorTangentSpace2 : TEXCOORD8;
    [[vk::location(10)]] float3 toLightVectorTangentSpace3 : TEXCOORD9;
    [[vk::location(11)]] float clipDistance : TEXCOORD10;
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

    float4 worldPosition = mul(uboVS.modelMatrix, float4(input.position, 1.0));
    output.worldPosition = worldPosition.xyz;
    output.clipDistance = dot(worldPosition, uboVS.clipPlane);

    float4 viewPosition = mul(uboVS.viewMatrices[viewIndex], worldPosition);
    output.viewPosition = viewPosition.xyz;
    output.position = mul(uboVS.projectionMatrices[viewIndex], viewPosition);

    output.textureCoord = (input.textureCoord / uboVS.textureNumberOfRows) + uboVS.textureOffset.xy;
    output.normal = mul(uboVS.normalMatrix, float4(input.normal, 0.0)).xyz;
    output.visibility = GetVisibility(viewPosition.xyz, uboVS.gradient, uboVS.density);

    float3x3 TBN = CreateTBNMatrix((float3x3)uboVS.modelMatrix, input.normal, input.tangent, input.biTangent);

    output.toCameraVectorTangentSpace = mul(TBN, uboVS.cameraPositions[viewIndex].xyz);
    output.positionTangentSpace = mul(TBN, worldPosition.xyz);

    float3 toLightVectors[4];
    for (int i = 0; i < (int)uboVS.lightning.realCountOfLights; i++)
    {
        toLightVectors[i] = mul(TBN, uboVS.lightning.lights[i].position.xyz);
    }

    output.toLightVectorTangentSpace0 = toLightVectors[0];
    output.toLightVectorTangentSpace1 = toLightVectors[1];
    output.toLightVectorTangentSpace2 = toLightVectors[2];
    output.toLightVectorTangentSpace3 = toLightVectors[3];
    return output;
}

[shader("fragment")]
float4 fragmentMain(Interpolants input) : SV_Target
{
    float3 toLightVectors[4] = { input.toLightVectorTangentSpace0, input.toLightVectorTangentSpace1, input.toLightVectorTangentSpace2, input.toLightVectorTangentSpace3 };
    if (input.clipDistance < 0.0)
    {
        discard;
    }

    float shadow = 1.0;
    if (uboFS.castedByShadows != 0)
    {
        shadow = GetShadow(depthTexture, depthSampler, uboFS.shadows, input.viewPosition, input.worldPosition, 0.005, input.position);
    }

    float3 normal = uboFS.hasNormalMap != 0 ? NormalMapping(normalTexture, normalSampler, input.textureCoord) : input.normal;
    float4 textureColor = colorTexture.Sample(colorSampler, input.textureCoord);
    float3 unitToCameraVector = normalize(input.toCameraVectorTangentSpace - input.positionTangentSpace);

    float3 totalDiffuse = float3(0.0);
    float3 totalSpecular = float3(0.0);
    for (uint i = 0; i < uboFS.lightning.realCountOfLights; i++)
    {
        Light light = uboFS.lightning.lights[i];
        float3 toLightVector = toLightVectors[i] - input.positionTangentSpace;
        float3 unitToLightVector = normalize(toLightVector);
        float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
        totalDiffuse += GetDiffuseColor(normal, unitToLightVector, light.color.xyz, attenuationFactor);
        totalSpecular += GetSpecularColor(normal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, uboFS.material.shineDamper, uboFS.material.reflectivity);
    }
    totalDiffuse = totalDiffuse * shadow + uboFS.lightning.ambientFactor;
    totalSpecular = totalSpecular * shadow;

    float4 baseResultColor = float4(totalDiffuse, 1.0) * textureColor + float4(totalSpecular, 0.0);
    float4 resultColor = lerp(float4(uboFS.fogColor.xyz, 1.0), baseResultColor, input.visibility);

    if (uboFS.selected != 0)
    {
        resultColor = lerp(resultColor, uboFS.selectedColor, 0.5);
    }

    return resultColor;
}
