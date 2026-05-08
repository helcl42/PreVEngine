struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

struct UniformBufferObject {
    modelMatrix: mat4x4<f32>,
    viewMatrix: mat4x4<f32>,
    projectionMatrix: mat4x4<f32>,
}

var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;

fn main_1() {
    let _e11 = ubo.projectionMatrix;
    let _e13 = ubo.viewMatrix;
    let _e16 = ubo.modelMatrix;
    let _e18 = inPosition_1;
    unnamed.gl_Position = (((_e11 * _e13) * _e16) * vec4<f32>(_e18.x, _e18.y, _e18.z, 1f));
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
