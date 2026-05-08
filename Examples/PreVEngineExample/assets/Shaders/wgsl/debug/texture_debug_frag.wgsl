@group(0) @binding(0) 
var imageTexture: texture_2d<f32>;
@group(0) @binding(1) 
var imageSampler: sampler;
var<private> textureCoord_1: vec2<f32>;
var<private> outFragColor: vec4<f32>;

fn main_1() {
    var color: vec4<f32>;

    let _e5 = textureCoord_1;
    let _e6 = textureSample(imageTexture, imageSampler, _e5);
    color = _e6;
    let _e7 = color;
    outFragColor = _e7;
    return;
}

@fragment 
fn main(@location(0) textureCoord: vec2<f32>) -> @location(0) vec4<f32> {
    textureCoord_1 = textureCoord;
    main_1();
    let _e3 = outFragColor;
    return _e3;
}
