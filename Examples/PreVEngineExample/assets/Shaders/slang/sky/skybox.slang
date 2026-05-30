// Skybox shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct SkyboxVSParams
{
    float4x4 modelMatrix;
    float4x4 viewMatrices[MAX_VIEW_COUNT];
    float4x4 projectionMatrices[MAX_VIEW_COUNT];
};

struct SkyboxFSParams
{
    float4 fogColor;
    float4 lowerLimit;
    float4 upperLimit;
};

[[vk::binding(0)]] ConstantBuffer<SkyboxVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<SkyboxFSParams> uboFS;
[[vk::binding(2)]] TextureCube cubeMap1Texture;
[[vk::binding(3)]] SamplerState cubeMap1Sampler;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float3 textureCoord : TEXCOORD0;
};

[shader("vertex")]
VertexOutput vertexMain(VertexInput input
#ifdef ENABLE_XR
    , uint viewIndex : SV_ViewID
#endif
)
{
#ifndef ENABLE_XR
    const int viewIndex = 0;
#endif
    VertexOutput output;
    output.position = mul(uboVS.projectionMatrices[viewIndex], mul(uboVS.viewMatrices[viewIndex], mul(uboVS.modelMatrix, float4(input.position, 1.0))));
    output.textureCoord = input.position;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    float3 tc = float3(-input.textureCoord.x, input.textureCoord.y, input.textureCoord.z);
    float4 finalColor = cubeMap1Texture.Sample(cubeMap1Sampler, tc);

    float factor = (input.textureCoord.y - uboFS.lowerLimit.x) / (uboFS.upperLimit.x - uboFS.lowerLimit.x);
    factor = clamp(factor, 0.0, 1.0);

    return lerp(float4(uboFS.fogColor.xyz, 1.0), finalColor, factor);
}
