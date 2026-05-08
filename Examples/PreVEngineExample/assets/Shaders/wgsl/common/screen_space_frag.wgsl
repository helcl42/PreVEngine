@group(0) @binding(0) 
var imageTexture: texture_2d<f32>;
@group(0) @binding(1) 
var imageSampler: sampler;
var<private> inTextureCoord_1: vec2<f32>;
var<private> outColor: vec4<f32>;

fn main_1() {
    var color: vec4<f32>;

    let _e5 = inTextureCoord_1;
    let _e6 = textureSample(imageTexture, imageSampler, _e5);
    color = _e6;
    let _e7 = color;
    outColor = _e7;
    return;
}

@fragment 
fn main(@location(0) inTextureCoord: vec2<f32>) -> @location(0) vec4<f32> {
    inTextureCoord_1 = inTextureCoord;
    main_1();
    let _e3 = outColor;
    return _e3;
}
