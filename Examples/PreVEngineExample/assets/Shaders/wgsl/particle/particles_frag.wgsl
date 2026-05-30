@binding(2) @group(0) var colorTexture_0 : texture_2d<f32>;

@binding(3) @group(0) var colorSampler_0 : sampler;

struct ParticleFSParams_std140_0
{
    @align(16) color_0 : vec4<f32>,
};

@binding(1) @group(0) var<uniform> uboFS_0 : ParticleFSParams_std140_0;
struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

struct pixelInput_0
{
    @location(0) currentStageTextureCoord_0 : vec2<f32>,
    @location(1) nextStageTextureCoord_0 : vec2<f32>,
    @location(2) currentNextStageBlendFactor_0 : f32,
};

@fragment
fn fragmentMain( _S1 : pixelInput_0, @builtin(position) position_0 : vec4<f32>) -> pixelOutput_0
{
    var _S2 : pixelOutput_0 = pixelOutput_0( uboFS_0.color_0 * mix((textureSample((colorTexture_0), (colorSampler_0), (_S1.currentStageTextureCoord_0))), (textureSample((colorTexture_0), (colorSampler_0), (_S1.nextStageTextureCoord_0))), vec4<f32>(_S1.currentNextStageBlendFactor_0)) );
    return _S2;
}

