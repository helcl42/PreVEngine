// Sky composite shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

#ifdef ENABLE_XR
[[vk::binding(0)]] Texture2D colorTex[MAX_VIEW_COUNT];
[[vk::binding(1)]] SamplerState colorSampler;
[[vk::binding(2)]] Texture2D depthTex[MAX_VIEW_COUNT];
[[vk::binding(3)]] SamplerState depthSampler;
#else
[[vk::binding(0)]] Texture2D colorTex;
[[vk::binding(1)]] SamplerState colorSampler;
[[vk::binding(2)]] Texture2D depthTex;
[[vk::binding(3)]] SamplerState depthSampler;
#endif

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float2 textureCoord : TEXCOORD0;
};

[shader("vertex")]
VertexOutput vertexMain(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.textureCoord = input.textureCoord;
    return output;
}

struct FragmentOutput
{
    float4 color : SV_Target;
    float depth : SV_Depth;
};

[shader("fragment")]
FragmentOutput fragmentMain(VertexOutput input
#ifdef ENABLE_XR
    , uint viewIndex : SV_ViewID
#endif
)
{
#ifndef ENABLE_XR
    const int viewIndex = 0;
#endif
    FragmentOutput output;
    float2 uv = input.textureCoord;

#ifdef ENABLE_XR
    output.color = colorTex[viewIndex].Sample(colorSampler, uv);
    output.depth = clamp(depthTex[viewIndex].Sample(depthSampler, uv).r, 0.0, 1.0);
#else
    output.color = colorTex.Sample(colorSampler, uv);
    output.depth = clamp(depthTex.Sample(depthSampler, uv).r, 0.0, 1.0);
#endif

    return output;
}
