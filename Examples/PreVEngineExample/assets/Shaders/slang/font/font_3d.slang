// Font 3D shader (world-space text)
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct Font3DVSParams
{
    float4x4 modelMatrix;
    float4x4 viewMatrices[MAX_VIEW_COUNT];
    float4x4 projectionMatrices[MAX_VIEW_COUNT];
    float4 clipPlane;
};

struct Font3DFSParams
{
    float4 color;
    float4 width;
    float4 edge;
    float4 bias;
    uint hasEffect;
    float4 borderWidth;
    float4 borderEdge;
    float4 outlineColor;
    float4 outlineOffset;
};

[[vk::binding(0)]] ConstantBuffer<Font3DVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<Font3DFSParams> uboFS;
[[vk::binding(2)]] Texture2D alphaTexture;
[[vk::binding(3)]] SamplerState alphaSampler;

struct VertexInput
{
    [[vk::location(0)]] float2 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
};

struct Interpolants
{
    float4 position : SV_Position;
    [[vk::location(0)]] float2 textureCoord : TEXCOORD0;
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
    output.position = mul(uboVS.projectionMatrices[viewIndex], mul(uboVS.viewMatrices[viewIndex], mul(uboVS.modelMatrix, float4(input.position.x, -input.position.y, 0.0, 1.0))));
    output.textureCoord = input.textureCoord;
    return output;
}

[shader("fragment")]
float4 fragmentMain(Interpolants input) : SV_Target
{
    float distance = 1.0 - alphaTexture.Sample(alphaSampler, float2(input.textureCoord.x + uboFS.bias.x, input.textureCoord.y + uboFS.bias.y)).a;
    float alpha = 1.0 - smoothstep(uboFS.width.x, uboFS.width.x + uboFS.edge.x, distance);

    if (uboFS.hasEffect != 0)
    {
        float distance2 = 1.0 - alphaTexture.Sample(alphaSampler, float2(input.textureCoord.x, input.textureCoord.y) + uboFS.outlineOffset.xy).a;
        float outlineAlpha = 1.0 - smoothstep(uboFS.borderWidth.x, uboFS.borderWidth.x + uboFS.borderEdge.x, distance2);

        float overallAlpha = alpha + (1.0 - alpha) * outlineAlpha;
        float3 overallColor = lerp(uboFS.outlineColor.xyz, uboFS.color.rgb, alpha / overallAlpha);

        return float4(overallColor, overallAlpha * uboFS.color.a);
    }
    else
    {
        return float4(uboFS.color.rgb, alpha * uboFS.color.a);
    }
}
