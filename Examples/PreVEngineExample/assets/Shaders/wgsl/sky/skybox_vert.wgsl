struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

struct UniformBufferObject {
    modelMatrix: mat4x4<f32>,
    viewMatrices: array<mat4x4<f32>, 1>,
    projectionMatrices: array<mat4x4<f32>, 1>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec3<f32>,
}

var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> outTextureCoord: vec3<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;

fn main_1() {
    let _e13 = uboVS.projectionMatrices[0i];
    let _e16 = uboVS.viewMatrices[0i];
    let _e19 = uboVS.modelMatrix;
    let _e21 = inPosition_1;
    unnamed.gl_Position = (((_e13 * _e16) * _e19) * vec4<f32>(_e21.x, _e21.y, _e21.z, 1f));
    let _e28 = inPosition_1;
    outTextureCoord = _e28;
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
    let _e13 = outTextureCoord;
    return VertexOutput(_e12, _e13);
}
