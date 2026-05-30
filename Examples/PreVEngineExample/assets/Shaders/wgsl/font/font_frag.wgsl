struct FontFSParams_std140_0
{
    @align(16) color_0 : vec4<f32>,
    @align(16) width_0 : vec4<f32>,
    @align(16) edge_0 : vec4<f32>,
    @align(16) bias_0 : vec4<f32>,
    @align(16) hasEffect_0 : u32,
    @align(16) borderWidth_0 : vec4<f32>,
    @align(16) borderEdge_0 : vec4<f32>,
    @align(16) outlineColor_0 : vec4<f32>,
    @align(16) outlineOffset_0 : vec4<f32>,
};

@binding(1) @group(0) var<uniform> uboFS_0 : FontFSParams_std140_0;
@binding(2) @group(0) var alphaTexture_0 : texture_2d<f32>;

@binding(3) @group(0) var alphaSampler_0 : sampler;

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
    var _S2 : f32 = _S1.textureCoord_0.x;
    var _S3 : f32 = _S1.textureCoord_0.y;
    var alpha_0 : f32 = 1.0f - smoothstep(uboFS_0.width_0.x, uboFS_0.width_0.x + uboFS_0.edge_0.x, 1.0f - (textureSample((alphaTexture_0), (alphaSampler_0), (vec2<f32>(_S2 + uboFS_0.bias_0.x, _S3 + uboFS_0.bias_0.y)))).w);
    if((uboFS_0.hasEffect_0) != u32(0))
    {
        var overallAlpha_0 : f32 = alpha_0 + (1.0f - alpha_0) * (1.0f - smoothstep(uboFS_0.borderWidth_0.x, uboFS_0.borderWidth_0.x + uboFS_0.borderEdge_0.x, 1.0f - (textureSample((alphaTexture_0), (alphaSampler_0), (vec2<f32>(_S2, _S3) + uboFS_0.outlineOffset_0.xy))).w));
        var _S4 : pixelOutput_0 = pixelOutput_0( vec4<f32>(mix(uboFS_0.outlineColor_0.xyz, uboFS_0.color_0.xyz, vec3<f32>((alpha_0 / overallAlpha_0))), overallAlpha_0 * uboFS_0.color_0.w) );
        return _S4;
    }
    else
    {
        var _S5 : pixelOutput_0 = pixelOutput_0( vec4<f32>(uboFS_0.color_0.xyz, alpha_0 * uboFS_0.color_0.w) );
        return _S5;
    }
}

