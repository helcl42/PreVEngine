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
    cameraPositions: array<vec4<f32>, 1>,
    density: f32,
    gradient: f32,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(2) member: vec3<f32>,
    @location(3) member_1: vec3<f32>,
    @location(0) member_2: vec4<f32>,
    @location(1) member_3: vec2<f32>,
    @location(4) member_4: vec3<f32>,
    @location(5) member_5: f32,
}

var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> outWorldPosition: vec3<f32>;
var<private> outViewPosition: vec3<f32>;
var<private> outClipSpaceCoord: vec4<f32>;
var<private> outTextureCoord: vec2<f32>;
var<private> outToCameraVector: vec3<f32>;
var<private> outVisibility: f32;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;

fn main_1() {
    var worldPosition: vec4<f32>;
    var viewPosition: vec4<f32>;
    var vertexToCameraDistance: f32;

    let _e27 = uboVS.projectionMatrices[0i];
    let _e30 = uboVS.viewMatrices[0i];
    let _e33 = uboVS.modelMatrix;
    let _e35 = inPosition_1;
    unnamed.gl_Position = (((_e27 * _e30) * _e33) * vec4<f32>(_e35.x, _e35.y, _e35.z, 1f));
    let _e43 = uboVS.modelMatrix;
    let _e44 = inPosition_1;
    worldPosition = (_e43 * vec4<f32>(_e44.x, _e44.y, _e44.z, 1f));
    let _e50 = worldPosition;
    outWorldPosition = _e50.xyz;
    let _e54 = uboVS.viewMatrices[0i];
    let _e55 = worldPosition;
    viewPosition = (_e54 * _e55);
    let _e57 = viewPosition;
    outViewPosition = _e57.xyz;
    let _e61 = uboVS.projectionMatrices[0i];
    let _e62 = viewPosition;
    outClipSpaceCoord = (_e61 * _e62);
    let _e64 = outClipSpaceCoord;
    unnamed.gl_Position = _e64;
    let _e66 = inPosition_1;
    let _e71 = ((_e66.xz / vec2(2f)) + vec2(0.5f));
    outTextureCoord = (vec2<f32>(_e71.x, _e71.y) * 1f);
    let _e78 = uboVS.cameraPositions[0i];
    let _e80 = worldPosition;
    outToCameraVector = (_e78.xyz - _e80.xyz);
    let _e83 = viewPosition;
    vertexToCameraDistance = length(_e83.xyz);
    let _e86 = vertexToCameraDistance;
    let _e88 = uboVS.density;
    let _e91 = uboVS.gradient;
    outVisibility = clamp(exp(-(pow((_e86 * _e88), _e91))), 0f, 1f);
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    main_1();
    let _e15 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e15);
    let _e17 = unnamed.gl_Position;
    let _e18 = outWorldPosition;
    let _e19 = outViewPosition;
    let _e20 = outClipSpaceCoord;
    let _e21 = outTextureCoord;
    let _e22 = outToCameraVector;
    let _e23 = outVisibility;
    return VertexOutput(_e17, _e18, _e19, _e20, _e21, _e22, _e23);
}
