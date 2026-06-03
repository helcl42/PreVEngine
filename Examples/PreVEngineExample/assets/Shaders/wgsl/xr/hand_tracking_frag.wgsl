struct HandTrackingFSParams_std140_0
{
    @align(16) color_0 : vec4<f32>,
};

@binding(1) @group(0) var<uniform> uboFS_0 : HandTrackingFSParams_std140_0;
struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

struct pixelInput_0
{
    @location(0) normal_0 : vec3<f32>,
    @location(1) worldPosition_0 : vec3<f32>,
};

@fragment
fn fragmentMain( _S1 : pixelInput_0, @builtin(position) position_0 : vec4<f32>) -> pixelOutput_0
{
    var _S2 : pixelOutput_0 = pixelOutput_0( vec4<f32>(uboFS_0.color_0.xyz * vec3<f32>((0.40000000596046448f + 0.60000002384185791f * max(dot(_S1.normal_0, normalize(vec3<f32>(0.5f, 1.0f, 0.30000001192092896f))), 0.0f))), uboFS_0.color_0.w) );
    return _S2;
}

