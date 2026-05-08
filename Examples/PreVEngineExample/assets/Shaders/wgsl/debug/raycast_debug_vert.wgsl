struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

struct UniformBufferObject {
    color: vec3<f32>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
}

var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> inPosition_1: vec3<f32>;
var<private> outColor: vec3<f32>;
@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;

fn main_1() {
    let _e9 = inPosition_1;
    unnamed.gl_Position = vec4<f32>(_e9.x, _e9.y, _e9.z, 1f);
    let _e16 = uboVS.color;
    outColor = _e16;
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
    let _e12 = unnamed.gl_Position;
    let _e13 = outColor;
    return VertexOutput(_e12, _e13);
}
