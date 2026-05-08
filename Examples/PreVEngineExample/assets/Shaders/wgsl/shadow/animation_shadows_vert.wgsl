struct UniformBufferObject {
    bones: array<mat4x4<f32>, 100>,
    modelMatrix: mat4x4<f32>,
    viewMatrix: mat4x4<f32>,
    projectionMatrix: mat4x4<f32>,
}

struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

@group(0) @binding(0) 
var<uniform> ubo: UniformBufferObject;
var<private> inBoneIds_1: vec4<i32>;
var<private> inWeights_1: vec4<f32>;
var<private> inPosition_1: vec3<f32>;
var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;

fn main_1() {
    var boneTransform: mat4x4<f32>;
    var positionL: vec4<f32>;

    let _e21 = inBoneIds_1[0u];
    let _e24 = ubo.bones[_e21];
    let _e26 = inWeights_1[0u];
    boneTransform = (_e24 * _e26);
    let _e29 = inBoneIds_1[1u];
    let _e32 = ubo.bones[_e29];
    let _e34 = inWeights_1[1u];
    let _e35 = (_e32 * _e34);
    let _e36 = boneTransform;
    boneTransform = mat4x4<f32>((_e36[0] + _e35[0]), (_e36[1] + _e35[1]), (_e36[2] + _e35[2]), (_e36[3] + _e35[3]));
    let _e51 = inBoneIds_1[2u];
    let _e54 = ubo.bones[_e51];
    let _e56 = inWeights_1[2u];
    let _e57 = (_e54 * _e56);
    let _e58 = boneTransform;
    boneTransform = mat4x4<f32>((_e58[0] + _e57[0]), (_e58[1] + _e57[1]), (_e58[2] + _e57[2]), (_e58[3] + _e57[3]));
    let _e73 = inBoneIds_1[3u];
    let _e76 = ubo.bones[_e73];
    let _e78 = inWeights_1[3u];
    let _e79 = (_e76 * _e78);
    let _e80 = boneTransform;
    boneTransform = mat4x4<f32>((_e80[0] + _e79[0]), (_e80[1] + _e79[1]), (_e80[2] + _e79[2]), (_e80[3] + _e79[3]));
    let _e94 = boneTransform;
    let _e95 = inPosition_1;
    positionL = (_e94 * vec4<f32>(_e95.x, _e95.y, _e95.z, 1f));
    let _e102 = ubo.projectionMatrix;
    let _e104 = ubo.viewMatrix;
    let _e107 = ubo.modelMatrix;
    let _e109 = positionL;
    let _e110 = _e109.xyz;
    unnamed.gl_Position = (((_e102 * _e104) * _e107) * vec4<f32>(_e110.x, _e110.y, _e110.z, 1f));
    return;
}

@vertex 
fn main(@location(3) inBoneIds: vec4<i32>, @location(4) inWeights: vec4<f32>, @location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>) -> @builtin(position) vec4<f32> {
    inBoneIds_1 = inBoneIds;
    inWeights_1 = inWeights;
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    main_1();
    let _e13 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e13);
    let _e15 = unnamed.gl_Position;
    return _e15;
}
