struct UniformBufferObject {
    translations: array<vec4<f32>, 1>,
    scale: vec4<f32>,
}

struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;

fn main_1() {
    var translation: vec3<f32>;
    var screenPosition: vec2<f32>;

    let _e14 = uboVS.translations[0i];
    translation = _e14.xyz;
    let _e16 = inPosition_1;
    let _e19 = uboVS.scale;
    let _e22 = translation;
    screenPosition = ((_e16.xy * _e19.xy) + _e22.xy);
    let _e25 = screenPosition;
    let _e27 = translation[2u];
    unnamed.gl_Position = vec4<f32>(_e25.x, _e25.y, _e27, 1f);
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>) -> @builtin(position) vec4<f32> {
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    main_1();
    let _e9 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e9);
    let _e11 = unnamed.gl_Position;
    return _e11;
}
