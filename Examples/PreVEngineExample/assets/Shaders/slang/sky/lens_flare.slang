// Lens flare shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct LensFlareVSParams
{
    float4 translations[MAX_VIEW_COUNT];
    float4 scale;
};

struct LensFlareFSParams
{
    float4 brightness;
};

[[vk::binding(0)]] ConstantBuffer<LensFlareVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<LensFlareFSParams> uboFS;
[[vk::binding(2)]] Texture2D colorTexture;
[[vk::binding(3)]] SamplerState colorSampler;

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
    output.textureCoord = input.position.xy + float2(0.5, 0.5);

    float3 translation = uboVS.translations[viewIndex].xyz;
    float2 screenPosition = input.position.xy * uboVS.scale.xy + translation.xy;
    output.position = float4(screenPosition, translation.z, 1.0);
    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    float4 color = colorTexture.Sample(colorSampler, input.textureCoord);
    color.a *= uboFS.brightness.x;
    return color;
}
