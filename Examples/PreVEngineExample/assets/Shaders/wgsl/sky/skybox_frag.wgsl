@binding(2) @group(0) var cubeMap1Texture_0 : texture_cube<f32>;

@binding(3) @group(0) var cubeMap1Sampler_0 : sampler;

struct SkyboxFSParams_std140_0
{
    @align(16) fogColor_0 : vec4<f32>,
    @align(16) lowerLimit_0 : vec4<f32>,
    @align(16) upperLimit_0 : vec4<f32>,
};

@binding(1) @group(0) var<uniform> uboFS_0 : SkyboxFSParams_std140_0;
struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

struct pixelInput_0
{
    @location(0) textureCoord_0 : vec3<f32>,
};

@fragment
fn fragmentMain( _S1 : pixelInput_0, @builtin(position) position_0 : vec4<f32>) -> pixelOutput_0
{
    var _S2 : f32 = _S1.textureCoord_0.y;
    var _S3 : pixelOutput_0 = pixelOutput_0( mix(vec4<f32>(uboFS_0.fogColor_0.xyz, 1.0f), (textureSample((cubeMap1Texture_0), (cubeMap1Sampler_0), (vec3<f32>(- _S1.textureCoord_0.x, _S2, _S1.textureCoord_0.z)))), vec4<f32>(clamp((_S2 - uboFS_0.lowerLimit_0.x) / (uboFS_0.upperLimit_0.x - uboFS_0.lowerLimit_0.x), 0.0f, 1.0f))) );
    return _S3;
}

