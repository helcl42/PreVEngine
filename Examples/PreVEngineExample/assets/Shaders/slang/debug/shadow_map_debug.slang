// Shadow map debug visualization shader
struct PushConsts
{
    uint cascadeIndex;
    float nearClippingPlane;
    float farClippingPlane;
};

[[vk::binding(0)]] Texture2DArray depthTexture;
[[vk::binding(1)]] SamplerState depthSampler;
[[vk::push_constant]] PushConsts pushConsts;

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

float LinearizeDepth(float near, float far, float depth)
{
    return (2.0 * near) / ((far + near) - (depth * (far - near)));
}

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    float depth = depthTexture.Sample(depthSampler, float3(input.textureCoord, float(pushConsts.cascadeIndex))).r;
    float linearDepth = LinearizeDepth(pushConsts.nearClippingPlane, pushConsts.farClippingPlane, depth);
    float value = 1.0 - linearDepth;
    return float4(value, value, value, 1.0);
}
