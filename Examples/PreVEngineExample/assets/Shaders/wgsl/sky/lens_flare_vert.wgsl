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

struct VertexOutput {
    @location(0) member: vec2<f32>,
    @builtin(position) gl_Position: vec4<f32>,
}

var<private> outTextureCoord: vec2<f32>;
var<private> inPosition_1: vec3<f32>;
@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;

fn main_1() {
    var translation: vec3<f32>;
    var screenPosition: vec2<f32>;

    let _e15 = inPosition_1;
    outTextureCoord = (_e15.xy + vec2<f32>(0.5f, 0.5f));
    let _e20 = uboVS.translations[0i];
    translation = _e20.xyz;
    let _e22 = inPosition_1;
    let _e25 = uboVS.scale;
    let _e28 = translation;
    screenPosition = ((_e22.xy * _e25.xy) + _e28.xy);
    let _e31 = screenPosition;
    let _e33 = translation[2u];
    unnamed.gl_Position = vec4<f32>(_e31.x, _e31.y, _e33, 1f);
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    main_1();
    let _e10 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e10);
    let _e12 = outTextureCoord;
    let _e13 = unnamed.gl_Position;
    return VertexOutput(_e12, _e13);
}
