struct PushConsts_std430_0
{
    @align(4) cascadeIndex_0 : u32,
    @align(4) nearClippingPlane_0 : f32,
    @align(4) farClippingPlane_0 : f32,
};

var<uniform> pushConsts_0 : PushConsts_std430_0;
@binding(0) @group(0) var depthTexture_0 : texture_2d_array<f32>;

@binding(1) @group(0) var depthSampler_0 : sampler;

fn LinearizeDepth_0( near_0 : f32,  far_0 : f32,  depth_0 : f32) -> f32
{
    return 2.0f * near_0 / (far_0 + near_0 - depth_0 * (far_0 - near_0));
}

struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

struct pixelInput_0
{
    @location(0) textureCoord_0 : vec2<f32>,
};

@fragment
fn fragmentMain( _S1 : pixelInput_0, @builtin(position) position_0 : vec4<f32>) -> pixelOutput_0
{
    var _S2 : vec3<f32> = vec3<f32>(_S1.textureCoord_0, f32(pushConsts_0.cascadeIndex_0));
    var value_0 : f32 = 1.0f - LinearizeDepth_0(pushConsts_0.nearClippingPlane_0, pushConsts_0.farClippingPlane_0, (textureSample((depthTexture_0), (depthSampler_0), ((_S2)).xy, i32(((_S2)).z))).x);
    var _S3 : pixelOutput_0 = pixelOutput_0( vec4<f32>(value_0, value_0, value_0, 1.0f) );
    return _S3;
}

