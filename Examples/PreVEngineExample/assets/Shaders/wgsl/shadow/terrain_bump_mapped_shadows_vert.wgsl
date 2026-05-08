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
var<private> inTangent_1: vec3<f32>;
var<private> inBiTnagent_1: vec3<f32>;

fn main_1() {
    let _e13 = ubo.projectionMatrix;
    let _e15 = ubo.viewMatrix;
    let _e18 = ubo.modelMatrix;
    let _e20 = inPosition_1;
    unnamed.gl_Position = (((_e13 * _e15) * _e18) * vec4<f32>(_e20.x, _e20.y, _e20.z, 1f));
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>, @location(3) inTangent: vec3<f32>, @location(4) inBiTnagent: vec3<f32>) -> @builtin(position) vec4<f32> {
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inBiTnagent_1 = inBiTnagent;
    main_1();
    let _e13 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e13);
    let _e15 = unnamed.gl_Position;
    return _e15;
}
