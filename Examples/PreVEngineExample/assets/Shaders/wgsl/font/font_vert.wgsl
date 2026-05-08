struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

struct UniformBufferObject {
    translation: vec4<f32>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec2<f32>,
}

var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> inPosition_1: vec2<f32>;
@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> outTextureCoord: vec2<f32>;
var<private> inTextureCoord_1: vec2<f32>;

fn main_1() {
    let _e11 = inPosition_1;
    let _e13 = uboVS.translation;
    let _e16 = (_e11 + (_e13.xy * vec2<f32>(2f, 2f)));
    unnamed.gl_Position = vec4<f32>(_e16.x, _e16.y, 0f, 1f);
    let _e21 = inTextureCoord_1;
    outTextureCoord = _e21;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec2<f32>, @location(1) inTextureCoord: vec2<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    main_1();
    let _e8 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e8);
    let _e10 = unnamed.gl_Position;
    let _e11 = outTextureCoord;
    return VertexOutput(_e10, _e11);
}
