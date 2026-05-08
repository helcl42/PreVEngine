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
    clipPlane: vec4<f32>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec2<f32>,
}

var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> inPosition_1: vec2<f32>;
var<private> outTextureCoord: vec2<f32>;
var<private> inTextureCoord_1: vec2<f32>;

fn main_1() {
    let _e14 = uboVS.projectionMatrices[0i];
    let _e17 = uboVS.viewMatrices[0i];
    let _e20 = uboVS.modelMatrix;
    let _e23 = inPosition_1[0u];
    let _e25 = inPosition_1[1u];
    unnamed.gl_Position = (((_e14 * _e17) * _e20) * vec4<f32>(_e23, -(_e25), 0f, 1f));
    let _e30 = inTextureCoord_1;
    outTextureCoord = _e30;
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
