// Shadow pass - animated bump-mapped geometry
static const int MAX_BONES_COUNT = 100;

struct AnimBumpShadowParams
{
    float4x4 bones[MAX_BONES_COUNT];
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

[[vk::binding(0)]] ConstantBuffer<AnimBumpShadowParams> ubo;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
    [[vk::location(3)]] int4 boneIds : BLENDINDICES;
    [[vk::location(4)]] float4 weights : BLENDWEIGHT;
    [[vk::location(5)]] float3 tangent : TANGENT;
    [[vk::location(6)]] float3 biTangent : BINORMAL;
};

[shader("vertex")]
float4 vertexMain(VertexInput input) : SV_Position
{
    float4x4 boneTransform = ubo.bones[input.boneIds[0]] * input.weights[0];
    boneTransform += ubo.bones[input.boneIds[1]] * input.weights[1];
    boneTransform += ubo.bones[input.boneIds[2]] * input.weights[2];
    boneTransform += ubo.bones[input.boneIds[3]] * input.weights[3];

    float4 positionL = mul(boneTransform, float4(input.position, 1.0));
    return mul(ubo.projectionMatrix, mul(ubo.viewMatrix, mul(ubo.modelMatrix, float4(positionL.xyz, 1.0))));
}
