// Screen-space full-screen pass shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
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

[[vk::binding(0)]] Texture2D imageTexture;
[[vk::binding(1)]] SamplerState imageSampler;

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    return imageTexture.Sample(imageSampler, input.textureCoord);
}
