// Hand tracking visualization shader - renders cubes at hand joint positions

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct HandTrackingVSParams
{
    float4x4 modelMatrix;
    float4x4 viewMatrices[MAX_VIEW_COUNT];
    float4x4 projectionMatrices[MAX_VIEW_COUNT];
};

struct HandTrackingFSParams
{
    float4 color;
};

[[vk::binding(0)]] ConstantBuffer<HandTrackingVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<HandTrackingFSParams> uboFS;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
};

struct Interpolants
{
    float4 position : SV_Position;
    [[vk::location(0)]] float3 normal : NORMAL;
    [[vk::location(1)]] float3 worldPosition : TEXCOORD0;
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
    output.normal = normalize(mul((float3x3)uboVS.modelMatrix, input.normal));
    output.position = mul(uboVS.projectionMatrices[viewIndex], mul(uboVS.viewMatrices[viewIndex], worldPosition));

    return output;
}

[shader("fragment")]
float4 fragmentMain(Interpolants input) : SV_Target
{
    // Simple directional lighting for depth perception
    float3 lightDir = normalize(float3(0.5, 1.0, 0.3));
    float ndotl = max(dot(input.normal, lightDir), 0.0);
    float lighting = 0.4 + 0.6 * ndotl; // ambient + diffuse

    return float4(uboFS.color.rgb * lighting, uboFS.color.a);
}
