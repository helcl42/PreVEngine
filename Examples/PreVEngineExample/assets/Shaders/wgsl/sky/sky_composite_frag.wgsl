@binding(0) @group(0) var colorTex_0 : texture_2d<f32>;

@binding(1) @group(0) var colorSampler_0 : sampler;

@binding(2) @group(0) var depthTex_0 : texture_2d<f32>;

@binding(3) @group(0) var depthSampler_0 : sampler;

struct FragmentOutput_0
{
    @location(0) color_0 : vec4<f32>,
    @builtin(frag_depth) depth_0 : f32,
};

struct pixelInput_0
{
    @location(0) textureCoord_0 : vec2<f32>,
};

@fragment
fn fragmentMain( _S1 : pixelInput_0, @builtin(position) position_0 : vec4<f32>) -> FragmentOutput_0
{
    var output_0 : FragmentOutput_0;
    output_0.color_0 = (textureSample((colorTex_0), (colorSampler_0), (_S1.textureCoord_0)));
    output_0.depth_0 = (textureSample((depthTex_0), (depthSampler_0), (_S1.textureCoord_0))).x;
    return output_0;
}

