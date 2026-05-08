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
var<private> inTangent_1: vec3<f32>;
var<private> inBiTnagent_1: vec3<f32>;

fn main_1() {
    var boneTransform: mat4x4<f32>;
    var positionL: vec4<f32>;

    let _e23 = inBoneIds_1[0u];
    let _e26 = ubo.bones[_e23];
    let _e28 = inWeights_1[0u];
    boneTransform = (_e26 * _e28);
    let _e31 = inBoneIds_1[1u];
    let _e34 = ubo.bones[_e31];
    let _e36 = inWeights_1[1u];
    let _e37 = (_e34 * _e36);
    let _e38 = boneTransform;
    boneTransform = mat4x4<f32>((_e38[0] + _e37[0]), (_e38[1] + _e37[1]), (_e38[2] + _e37[2]), (_e38[3] + _e37[3]));
    let _e53 = inBoneIds_1[2u];
    let _e56 = ubo.bones[_e53];
    let _e58 = inWeights_1[2u];
    let _e59 = (_e56 * _e58);
    let _e60 = boneTransform;
    boneTransform = mat4x4<f32>((_e60[0] + _e59[0]), (_e60[1] + _e59[1]), (_e60[2] + _e59[2]), (_e60[3] + _e59[3]));
    let _e75 = inBoneIds_1[3u];
    let _e78 = ubo.bones[_e75];
    let _e80 = inWeights_1[3u];
    let _e81 = (_e78 * _e80);
    let _e82 = boneTransform;
    boneTransform = mat4x4<f32>((_e82[0] + _e81[0]), (_e82[1] + _e81[1]), (_e82[2] + _e81[2]), (_e82[3] + _e81[3]));
    let _e96 = boneTransform;
    let _e97 = inPosition_1;
    positionL = (_e96 * vec4<f32>(_e97.x, _e97.y, _e97.z, 1f));
    let _e104 = ubo.projectionMatrix;
    let _e106 = ubo.viewMatrix;
    let _e109 = ubo.modelMatrix;
    let _e111 = positionL;
    let _e112 = _e111.xyz;
    unnamed.gl_Position = (((_e104 * _e106) * _e109) * vec4<f32>(_e112.x, _e112.y, _e112.z, 1f));
    return;
}

@vertex 
fn main(@location(3) inBoneIds: vec4<i32>, @location(4) inWeights: vec4<f32>, @location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>, @location(5) inTangent: vec3<f32>, @location(6) inBiTnagent: vec3<f32>) -> @builtin(position) vec4<f32> {
    inBoneIds_1 = inBoneIds;
    inWeights_1 = inWeights;
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inBiTnagent_1 = inBiTnagent;
    main_1();
    let _e17 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e17);
    let _e19 = unnamed.gl_Position;
    return _e19;
}
