@binding(2) @group(0) var colorTexture_0 : texture_2d<f32>;

@binding(3) @group(0) var colorSampler_0 : sampler;

struct LensFlareFSParams_std140_0
{
    @align(16) brightness_0 : vec4<f32>,
};

@binding(1) @group(0) var<uniform> uboFS_0 : LensFlareFSParams_std140_0;
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
    var color_0 : vec4<f32> = (textureSample((colorTexture_0), (colorSampler_0), (_S1.textureCoord_0)));
    color_0[i32(3)] = color_0[i32(3)] * uboFS_0.brightness_0.x;
    var _S2 : pixelOutput_0 = pixelOutput_0( color_0 );
    return _S2;
}

