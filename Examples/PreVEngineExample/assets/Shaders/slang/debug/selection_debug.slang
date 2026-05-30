// Selection debug visualization shader
struct SelectionVSParams
{
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct SelectionFSParams
{
    float4 color;
};

[[vk::binding(0)]] ConstantBuffer<SelectionVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<SelectionFSParams> uboFS;

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
VertexOutput vertexMain(VertexInput input)
{
    VertexOutput output;
    output.position = mul(uboVS.projectionMatrix, mul(uboVS.viewMatrix, mul(uboVS.modelMatrix, float4(input.position, 1.0))));
    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    return uboFS.color;
}
