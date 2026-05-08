struct UniformBufferObject {
    color: vec4<f32>,
}

@group(0) @binding(2) 
var colorTexture: texture_2d<f32>;
@group(0) @binding(3) 
var colorSampler: sampler;
var<private> inCurrentStageTextureCoord_1: vec2<f32>;
var<private> inNextStageTextureCoord_1: vec2<f32>;
var<private> outColor: vec4<f32>;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> inCurrentNextStageBlendFactor_1: f32;

fn main_1() {
    var stage1Color: vec4<f32>;
    var stage2Color: vec4<f32>;

    let _e10 = inCurrentStageTextureCoord_1;
    let _e11 = textureSample(colorTexture, colorSampler, _e10);
    stage1Color = _e11;
    let _e12 = inNextStageTextureCoord_1;
    let _e13 = textureSample(colorTexture, colorSampler, _e12);
    stage2Color = _e13;
    let _e15 = uboFS.color;
    let _e16 = stage1Color;
    let _e17 = stage2Color;
    let _e18 = inCurrentNextStageBlendFactor_1;
    outColor = (_e15 * mix(_e16, _e17, vec4(_e18)));
    return;
}

@fragment 
fn main(@location(0) inCurrentStageTextureCoord: vec2<f32>, @location(1) inNextStageTextureCoord: vec2<f32>, @location(2) inCurrentNextStageBlendFactor: f32) -> @location(0) vec4<f32> {
    inCurrentStageTextureCoord_1 = inCurrentStageTextureCoord;
    inNextStageTextureCoord_1 = inNextStageTextureCoord;
    inCurrentNextStageBlendFactor_1 = inCurrentNextStageBlendFactor;
    main_1();
    let _e7 = outColor;
    return _e7;
}
