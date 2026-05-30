// Shadow pass - terrain bump-mapped geometry
struct ShadowParams
{
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

[[vk::binding(0)]] ConstantBuffer<ShadowParams> ubo;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
    [[vk::location(3)]] float3 tangent : TANGENT;
    [[vk::location(4)]] float3 biTangent : BINORMAL;
};

[shader("vertex")]
float4 vertexMain(VertexInput input) : SV_Position
{
    return mul(ubo.projectionMatrix, mul(ubo.viewMatrix, mul(ubo.modelMatrix, float4(input.position, 1.0))));
}
