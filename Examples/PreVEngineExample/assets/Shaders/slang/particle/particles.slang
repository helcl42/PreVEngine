// Particle billboard shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct ParticleVSParams
{
    float4x4 viewMatrices[MAX_VIEW_COUNT];
    float4x4 projectionMatrices[MAX_VIEW_COUNT];
    uint textureNumberOfRows;
};

struct ParticleFSParams
{
    float4 color;
};

[[vk::binding(0)]] ConstantBuffer<ParticleVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<ParticleFSParams> uboFS;
[[vk::binding(2)]] Texture2D colorTexture;
[[vk::binding(3)]] SamplerState colorSampler;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(7)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(8)]] float3 normal : NORMAL;
    // Instance data
    [[vk::location(1)]] float3 instancePosition : TEXCOORD1;
    [[vk::location(2)]] float2 scale : TEXCOORD2;
    [[vk::location(3)]] float rotation : TEXCOORD3;
    [[vk::location(4)]] float2 textureOffsetsCurrent : TEXCOORD4;
    [[vk::location(5)]] float2 textureOffsetNext : TEXCOORD5;
    [[vk::location(6)]] float blendFactor : TEXCOORD6;
};

struct Interpolants
{
    float4 position : SV_Position;
    [[vk::location(0)]] float2 currentStageTextureCoord : TEXCOORD0;
    [[vk::location(1)]] float2 nextStageTextureCoord : TEXCOORD1;
    [[vk::location(2)]] float currentNextStageBlendFactor : TEXCOORD2;
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

    float4x4 viewMatrix = uboVS.viewMatrices[viewIndex];
    float4x4 projectionMatrix = uboVS.projectionMatrices[viewIndex];

    float3 cameraRightWorldSpace = viewMatrix[0].xyz;
    float3 cameraUpWorldSpace = viewMatrix[1].xyz;

    float2 localPosition = mul(MakeRotation(input.rotation), float2(input.position.x * input.scale.x, input.position.y * input.scale.y));
    float3 vertexPositionWorldspace = input.instancePosition + cameraRightWorldSpace * -localPosition.x + cameraUpWorldSpace * localPosition.y;

    output.position = mul(projectionMatrix, mul(viewMatrix, float4(vertexPositionWorldspace, 1.0)));

    float2 textureCoordBase = input.textureCoord / uboVS.textureNumberOfRows;
    output.currentStageTextureCoord = textureCoordBase + input.textureOffsetsCurrent;
    output.nextStageTextureCoord = textureCoordBase + input.textureOffsetNext;
    output.currentNextStageBlendFactor = input.blendFactor;
    return output;
}

[shader("fragment")]
float4 fragmentMain(Interpolants input) : SV_Target
{
    float4 stage1Color = colorTexture.Sample(colorSampler, input.currentStageTextureCoord);
    float4 stage2Color = colorTexture.Sample(colorSampler, input.nextStageTextureCoord);
    return uboFS.color * lerp(stage1Color, stage2Color, input.currentNextStageBlendFactor);
}
