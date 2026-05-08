struct FragmentOutput {
    @location(0) member: vec4<f32>,
    @builtin(frag_depth) member_1: f32,
}

var<private> inTextureCoord_1: vec2<f32>;
@group(0) @binding(0) 
var colorTex: texture_2d<f32>;
@group(0) @binding(1) 
var colorSampler: sampler;
@group(0) @binding(2) 
var depthTex: texture_2d<f32>;
@group(0) @binding(3) 
var depthSampler: sampler;
var<private> outFragColor: vec4<f32>;
var<private> gl_FragDepth: f32 = 0f;

fn main_1() {
    var uv: vec2<f32>;
    var color: vec4<f32>;
    var depth: f32;

    let _e11 = inTextureCoord_1;
    uv = _e11;
    let _e12 = uv;
    let _e13 = textureSample(colorTex, colorSampler, _e12);
    color = _e13;
    let _e14 = uv;
    let _e15 = textureSample(depthTex, depthSampler, _e14);
    depth = _e15.x;
    let _e17 = color;
    outFragColor = _e17;
    let _e18 = depth;
    gl_FragDepth = _e18;
    return;
}

@fragment 
fn main(@location(0) inTextureCoord: vec2<f32>) -> FragmentOutput {
    inTextureCoord_1 = inTextureCoord;
    main_1();
    let _e4 = outFragColor;
    let _e5 = gl_FragDepth;
    return FragmentOutput(_e4, _e5);
}
