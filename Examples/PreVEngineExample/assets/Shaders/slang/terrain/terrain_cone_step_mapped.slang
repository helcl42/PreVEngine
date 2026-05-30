// Terrain cone-step-mapped shader with multi-material height blending
import common.utils;
import common.common;
import common.lights;
import common.shadows;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

static const uint MATERIAL_COUNT = 4;

struct TerrainCSMVSParams
{
    float4x4 modelMatrix;
    float4x4 normalMatrix;
    float4x4 viewMatrices[MAX_VIEW_COUNT];
    float4x4 projectionMatrices[MAX_VIEW_COUNT];
    float4 cameraPositions[MAX_VIEW_COUNT];
    float4 clipPlane;
    Lightning lightning;
    float density;
    float gradient;
};

struct TerrainCSMFSParams
{
    Shadows shadows;
    Lightning lightning;
    Material material[MATERIAL_COUNT];
    float4 fogColor;
    float4 selectedColor;
    uint selected;
    uint castedByShadows;
    float minHeight;
    float maxHeight;
    float4 heightSteps[MATERIAL_COUNT];
    float4 heightScale[MATERIAL_COUNT];
    float heightTransitionRange;
    uint numLayers;
    uint hasNormalMap;
    uint hasConeMap;
};

[[vk::binding(0)]] ConstantBuffer<TerrainCSMVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<TerrainCSMFSParams> uboFS;
[[vk::binding(2)]] Texture2DArray colorTextures;
[[vk::binding(3)]] SamplerState colorSampler;
[[vk::binding(4)]] Texture2DArray normalTextures;
[[vk::binding(5)]] SamplerState normalSampler;
[[vk::binding(6)]] Texture2DArray heightTextures;
[[vk::binding(7)]] SamplerState heightSampler;
[[vk::binding(8)]] Texture2DArray depthTexture;
[[vk::binding(9)]] SamplerState depthSampler;

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

    output.textureCoord = input.textureCoord;
    output.normal = mul(uboVS.normalMatrix, float4(input.normal, 0.0)).xyz;
    output.visibility = GetVisibility(viewPosition.xyz, uboVS.gradient, uboVS.density);

    float3x3 mv3 = (float3x3)mul(uboVS.viewMatrices[viewIndex], uboVS.modelMatrix);
    float3x3 TBN = CreateTBNMatrix(mv3, input.normal, input.tangent, input.biTangent);

    float4 cameraPositionViewSpace4 = mul(uboVS.viewMatrices[viewIndex], float4(uboVS.cameraPositions[viewIndex].xyz, 1.0));
    float3 cameraPositionViewSpace = cameraPositionViewSpace4.xyz / cameraPositionViewSpace4.w;

    output.toCameraVectorTangentSpace = mul(TBN, cameraPositionViewSpace);
    output.positionTangentSpace = mul(TBN, viewPosition.xyz);

    float3 toLightVectors[4];
    for (int i = 0; i < (int)uboVS.lightning.realCountOfLights; i++)
    {
        float3 lightPosViewSpace = mul(uboVS.viewMatrices[viewIndex], float4(uboVS.lightning.lights[i].position.xyz, 1.0)).xyz;
        toLightVectors[i] = mul(TBN, lightPosViewSpace);
    }

    output.toLightVectorTangentSpace0 = toLightVectors[0];
    output.toLightVectorTangentSpace1 = toLightVectors[1];
    output.toLightVectorTangentSpace2 = toLightVectors[2];
    output.toLightVectorTangentSpace3 = toLightVectors[3];
    return output;
}

static const bool useDepthBias = true;

float3 GetRayDirection(float3 viewDirection, float heightScaleVal)
{
    float3 v = normalize(viewDirection);
    v.z = abs(v.z);
    if (useDepthBias)
    {
        float db = 1.0 - v.z;
        db *= db;
        db *= db;
        db = 1.0 - db * db;
        v.xy *= db;
    }
    v.xy *= heightScaleVal;
    return v;
}

float GetInverseHeight(float height)
{
    return 1.0 - height;
}

float2 sampleRelaxedConeStepMapping(uint idx, float heightScaleVal, uint numLayers, float2 uv, float2 ddxUV, float2 ddyUV, float3 texDir3D)
{
    const uint binarySteps = 6;

    float3 rayPos = float3(uv, 0.0);
    float3 rayDir = GetRayDirection(texDir3D, heightScaleVal);

    rayDir /= rayDir.z;
    float rayRatio = length(rayDir.xy);
    float3 pos = rayPos;
    for (uint i = 0; i < numLayers; ++i)
    {
        float2 heightAndCone = clamp(heightTextures.SampleGrad(heightSampler, float3(pos.xy, (float)idx), ddxUV, ddyUV).rg, 0.0, 1.0);
        float coneRatio = heightAndCone.g * heightAndCone.g;
        float height = GetInverseHeight(heightAndCone.r) - pos.z;
        float d = coneRatio * height / (rayRatio + coneRatio);
        pos += rayDir * d;
    }

    float3 bsRange = 0.5 * rayDir * pos.z;
    float3 bsPosition = rayPos + bsRange;
    for (uint j = 0; j < binarySteps; ++j)
    {
        float2 heightAndCone = clamp(heightTextures.SampleGrad(heightSampler, float3(bsPosition.xy, (float)idx), ddxUV, ddyUV).rg, 0.0, 1.0);
        bsRange *= 0.5;
        if (bsPosition.z < GetInverseHeight(heightAndCone.r))
        {
            bsPosition += bsRange;
        }
        else
        {
            bsPosition -= bsRange;
        }
    }
    return bsPosition.xy;
}

float4 sampleColorTextureGrad(uint idx, float2 uv, float2 ddxUV, float2 ddyUV)
{
    return colorTextures.SampleGrad(colorSampler, float3(uv, (float)idx), ddxUV, ddyUV);
}

float3 sampleNormalMap(uint idx, float2 uv, float2 ddxUV, float2 ddyUV)
{
    float3 n = normalTextures.SampleGrad(normalSampler, float3(uv, (float)idx), ddxUV, ddyUV).xyz;
    return normalize(2.0 * normalize(n) - 1.0);
}

[shader("fragment")]
float4 fragmentMain(Interpolants input) : SV_Target
{
    float3 toLightVectors[4] = { input.toLightVectorTangentSpace0, input.toLightVectorTangentSpace1, input.toLightVectorTangentSpace2, input.toLightVectorTangentSpace3 };
    if (input.clipDistance < 0.0)
    {
        discard;
    }

    float3 rayDirection = normalize(input.positionTangentSpace);

    float heightRange = abs(uboFS.maxHeight) + abs(uboFS.minHeight);
    float normalizedHeight = (input.worldPosition.y + abs(uboFS.minHeight)) / heightRange;

    float4 textureColor = float4(1.0, 1.0, 1.0, 1.0);
    float3 normal = float3(0.0, 1.0, 0.0);
    float shineDamper = 1.0;
    float reflectivity = 1.0;
    float2 ddxTC = ddx(input.textureCoord);
    float2 ddyTC = ddy(input.textureCoord);

    for (uint i = 0; i < MATERIAL_COUNT; i++)
    {
        if (i < MATERIAL_COUNT - 1)
        {
            if (normalizedHeight > uboFS.heightSteps[i].x - uboFS.heightTransitionRange && normalizedHeight < uboFS.heightSteps[i].x + uboFS.heightTransitionRange)
            {
                float ratio = (normalizedHeight - uboFS.heightSteps[i].x + uboFS.heightTransitionRange) / (2.0 * uboFS.heightTransitionRange);

                float2 uv1;
                float2 uv2;
                if (uboFS.hasConeMap != 0)
                {
                    uv1 = sampleRelaxedConeStepMapping(i, uboFS.heightScale[i].x, uboFS.numLayers, input.textureCoord, ddxTC, ddyTC, rayDirection);
                    uv2 = sampleRelaxedConeStepMapping(i + 1, uboFS.heightScale[i + 1].x, uboFS.numLayers, input.textureCoord, ddxTC, ddyTC, rayDirection);
                }
                else
                {
                    uv1 = input.textureCoord;
                    uv2 = input.textureCoord;
                }

                float3 normal1;
                float3 normal2;
                if (uboFS.hasNormalMap != 0)
                {
                    normal1 = sampleNormalMap(i, uv1, ddxTC, ddyTC);
                    normal2 = sampleNormalMap(i + 1, uv2, ddxTC, ddyTC);
                }
                else
                {
                    normal1 = input.normal;
                    normal2 = input.normal;
                }
                normal = lerp(normal1, normal2, ratio);

                float4 color1 = sampleColorTextureGrad(i, uv1, ddxTC, ddyTC);
                float4 color2 = sampleColorTextureGrad(i + 1, uv2, ddxTC, ddyTC);
                textureColor = lerp(color1, color2, ratio);

                shineDamper = lerp(uboFS.material[i].shineDamper, uboFS.material[i + 1].shineDamper, ratio);
                reflectivity = lerp(uboFS.material[i].reflectivity, uboFS.material[i + 1].reflectivity, ratio);
                break;
            }
            else if (normalizedHeight < uboFS.heightSteps[i].x - uboFS.heightTransitionRange)
            {
                float2 uv = uboFS.hasConeMap != 0 ? sampleRelaxedConeStepMapping(i, uboFS.heightScale[i].x, uboFS.numLayers, input.textureCoord, ddxTC, ddyTC, rayDirection) : input.textureCoord;
                normal = uboFS.hasNormalMap != 0 ? sampleNormalMap(i, uv, ddxTC, ddyTC) : input.normal;
                textureColor = sampleColorTextureGrad(i, uv, ddxTC, ddyTC);
                shineDamper = uboFS.material[i].shineDamper;
                reflectivity = uboFS.material[i].reflectivity;
                break;
            }
        }
        else
        {
            float2 uv = uboFS.hasConeMap != 0 ? sampleRelaxedConeStepMapping(i, uboFS.heightScale[i].x, uboFS.numLayers, input.textureCoord, ddxTC, ddyTC, rayDirection) : input.textureCoord;
            normal = uboFS.hasNormalMap != 0 ? sampleNormalMap(i, uv, ddxTC, ddyTC) : input.normal;
            textureColor = sampleColorTextureGrad(i, uv, ddxTC, ddyTC);
            shineDamper = uboFS.material[i].shineDamper;
            reflectivity = uboFS.material[i].reflectivity;
        }
    }

    float shadow = 1.0;
    if (uboFS.castedByShadows != 0)
    {
        shadow = GetShadow(depthTexture, depthSampler, uboFS.shadows, input.viewPosition, input.worldPosition, 0.005, input.position);
    }

    float3 unitToCameraVector = normalize(input.toCameraVectorTangentSpace - input.positionTangentSpace);

    float3 totalDiffuse = float3(0.0, 0.0, 0.0);
    float3 totalSpecular = float3(0.0, 0.0, 0.0);
    for (uint j = 0; j < uboFS.lightning.realCountOfLights; j++)
    {
        Light light = uboFS.lightning.lights[j];
        float3 toLightVector = toLightVectors[j] - input.positionTangentSpace;
        float3 unitToLightVector = normalize(toLightVector);
        float attenuationFactor = GetAttenuationFactor(light.attenuation.xyz, toLightVector);
        totalDiffuse += GetDiffuseColor(normal, unitToLightVector, light.color.xyz, attenuationFactor);
        totalSpecular += GetSpecularColor(normal, unitToLightVector, unitToCameraVector, light.color.xyz, attenuationFactor, shineDamper, reflectivity);
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
