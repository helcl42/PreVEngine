@binding(0) @group(0) var imageTexture_0 : texture_2d<f32>;

@binding(1) @group(0) var imageSampler_0 : sampler;

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
    var _S2 : pixelOutput_0 = pixelOutput_0( (textureSample((imageTexture_0), (imageSampler_0), (_S1.textureCoord_0))) );
    return _S2;
}

