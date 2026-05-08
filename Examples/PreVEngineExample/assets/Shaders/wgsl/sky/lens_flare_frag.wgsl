struct UniformBufferObject {
    brightness: vec4<f32>,
}

var<private> outColor: vec4<f32>;
@group(0) @binding(2) 
var colorTexture: texture_2d<f32>;
@group(0) @binding(3) 
var colorSampler: sampler;
var<private> inTextureCoord_1: vec2<f32>;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;

fn main_1() {
    let _e8 = inTextureCoord_1;
    let _e9 = textureSample(colorTexture, colorSampler, _e8);
    outColor = _e9;
    let _e12 = uboFS.brightness[0u];
    let _e14 = outColor[3u];
    outColor[3u] = (_e14 * _e12);
    return;
}

@fragment 
fn main(@location(0) inTextureCoord: vec2<f32>) -> @location(0) vec4<f32> {
    inTextureCoord_1 = inTextureCoord;
    main_1();
    let _e3 = outColor;
    return _e3;
}
