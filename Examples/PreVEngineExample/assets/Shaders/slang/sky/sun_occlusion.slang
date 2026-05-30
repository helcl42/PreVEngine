// Sun occlusion shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct SunOcclusionVSParams
{
    float4 translations[MAX_VIEW_COUNT];
    float4 scale;
};

[[vk::binding(0)]] ConstantBuffer<SunOcclusionVSParams> uboVS;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_Position;
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
    float3 translation = uboVS.translations[viewIndex].xyz;
    float2 screenPosition = input.position.xy * uboVS.scale.xy + translation.xy;
    output.position = float4(screenPosition, translation.z, 1.0);
    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    return float4(1.0, 1.0, 1.0, 0.0);
}
