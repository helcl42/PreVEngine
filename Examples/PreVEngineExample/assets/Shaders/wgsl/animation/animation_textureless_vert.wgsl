struct Light {
    position: vec4<f32>,
    color: vec4<f32>,
    attenuation: vec4<f32>,
}

struct Lightning {
    lights: array<Light, 4>,
    realCountOfLights: u32,
    ambientFactor: f32,
}

struct UniformBufferObject {
    bones: array<mat4x4<f32>, 100>,
    modelMatrix: mat4x4<f32>,
    normalMatrix: mat4x4<f32>,
    viewMatrices: array<mat4x4<f32>, 1>,
    projectionMatrices: array<mat4x4<f32>, 1>,
    cameraPositions: array<vec4<f32>, 1>,
    clipPlane: vec4<f32>,
    lightning: Lightning,
    density: f32,
    gradient: f32,
}

struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

struct VertexOutput {
    @location(2) member: vec3<f32>,
    @location(10) member_1: f32,
    @location(3) member_2: vec3<f32>,
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member_3: vec2<f32>,
    @location(1) member_4: vec3<f32>,
    @location(4) member_5: vec3<f32>,
    @location(5) member_6: f32,
    @location(6) member_7: vec3<f32>,
    @location(7) member_8: vec3<f32>,
    @location(8) member_9: vec3<f32>,
    @location(9) member_10: vec3<f32>,
}

@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> inBoneIds_1: vec4<i32>;
var<private> inWeights_1: vec4<f32>;
var<private> inPosition_1: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> outWorldPosition: vec3<f32>;
var<private> outClipDistance: f32;
var<private> outViewPosition: vec3<f32>;
var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> outTextureCoord: vec2<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> outNormal: vec3<f32>;
var<private> outToCameraVector: vec3<f32>;
var<private> outVisibility: f32;
var<private> outToLightVector0_: vec3<f32>;
var<private> outToLightVector1_: vec3<f32>;
var<private> outToLightVector2_: vec3<f32>;
var<private> outToLightVector3_: vec3<f32>;

fn GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b(viewPosition: ptr<function, vec3<f32>>, gradient: ptr<function, f32>, density: ptr<function, f32>) -> f32 {
    var vertexToCameraDistance: f32;
    var visibility: f32;

    let _e43 = (*viewPosition);
    vertexToCameraDistance = length(_e43);
    let _e45 = vertexToCameraDistance;
    let _e46 = (*density);
    let _e48 = (*gradient);
    visibility = exp(-(pow((_e45 * _e46), _e48)));
    let _e52 = visibility;
    return clamp(_e52, 0f, 1f);
}

fn main_1() {
    var boneTransform: mat4x4<f32>;
    var positionL: vec4<f32>;
    var normalL: vec4<f32>;
    var worldPosition: vec4<f32>;
    var viewPosition_1: vec4<f32>;
    var i: i32;
    var light: Light;
    var outToLightVector_arr: array<vec3<f32>, 4>;
    var param: vec3<f32>;
    var param_1: f32;
    var param_2: f32;

    let _e50 = inBoneIds_1[0u];
    let _e53 = uboVS.bones[_e50];
    let _e55 = inWeights_1[0u];
    boneTransform = (_e53 * _e55);
    let _e58 = inBoneIds_1[1u];
    let _e61 = uboVS.bones[_e58];
    let _e63 = inWeights_1[1u];
    let _e64 = (_e61 * _e63);
    let _e65 = boneTransform;
    boneTransform = mat4x4<f32>((_e65[0] + _e64[0]), (_e65[1] + _e64[1]), (_e65[2] + _e64[2]), (_e65[3] + _e64[3]));
    let _e80 = inBoneIds_1[2u];
    let _e83 = uboVS.bones[_e80];
    let _e85 = inWeights_1[2u];
    let _e86 = (_e83 * _e85);
    let _e87 = boneTransform;
    boneTransform = mat4x4<f32>((_e87[0] + _e86[0]), (_e87[1] + _e86[1]), (_e87[2] + _e86[2]), (_e87[3] + _e86[3]));
    let _e102 = inBoneIds_1[3u];
    let _e105 = uboVS.bones[_e102];
    let _e107 = inWeights_1[3u];
    let _e108 = (_e105 * _e107);
    let _e109 = boneTransform;
    boneTransform = mat4x4<f32>((_e109[0] + _e108[0]), (_e109[1] + _e108[1]), (_e109[2] + _e108[2]), (_e109[3] + _e108[3]));
    let _e123 = boneTransform;
    let _e124 = inPosition_1;
    positionL = (_e123 * vec4<f32>(_e124.x, _e124.y, _e124.z, 1f));
    let _e130 = boneTransform;
    let _e131 = inNormal_1;
    normalL = (_e130 * vec4<f32>(_e131.x, _e131.y, _e131.z, 0f));
    let _e138 = uboVS.modelMatrix;
    let _e139 = positionL;
    let _e140 = _e139.xyz;
    worldPosition = (_e138 * vec4<f32>(_e140.x, _e140.y, _e140.z, 1f));
    let _e146 = worldPosition;
    outWorldPosition = _e146.xyz;
    let _e148 = worldPosition;
    let _e150 = uboVS.clipPlane;
    outClipDistance = dot(_e148, _e150);
    let _e154 = uboVS.viewMatrices[0i];
    let _e155 = worldPosition;
    viewPosition_1 = (_e154 * _e155);
    let _e157 = viewPosition_1;
    outViewPosition = _e157.xyz;
    let _e161 = uboVS.projectionMatrices[0i];
    let _e162 = viewPosition_1;
    unnamed.gl_Position = (_e161 * _e162);
    let _e165 = inTextureCoord_1;
    outTextureCoord = _e165;
    let _e167 = uboVS.normalMatrix;
    let _e168 = normalL;
    let _e169 = _e168.xyz;
    outNormal = (_e167 * vec4<f32>(_e169.x, _e169.y, _e169.z, 0f)).xyz;
    i = 0i;
    loop {
        let _e176 = i;
        let _e180 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e176) < _e180) {
            let _e182 = i;
            let _e186 = uboVS.lightning.lights[_e182];
            light.position = _e186.position;
            light.color = _e186.color;
            light.attenuation = _e186.attenuation;
            let _e193 = i;
            let _e195 = light.position;
            let _e197 = worldPosition;
            outToLightVector_arr[_e193] = (_e195.xyz - _e197.xyz);
            continue;
        } else {
            break;
        }
        continuing {
            let _e201 = i;
            i = (_e201 + 1i);
        }
    }
    let _e205 = uboVS.cameraPositions[0i];
    let _e207 = worldPosition;
    outToCameraVector = (_e205.xyz - _e207.xyz);
    let _e210 = viewPosition_1;
    param = _e210.xyz;
    let _e213 = uboVS.gradient;
    param_1 = _e213;
    let _e215 = uboVS.density;
    param_2 = _e215;
    let _e216 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e216;
    let _e218 = outToLightVector_arr[0i];
    outToLightVector0_ = _e218;
    let _e220 = outToLightVector_arr[1i];
    outToLightVector1_ = _e220;
    let _e222 = outToLightVector_arr[2i];
    outToLightVector2_ = _e222;
    let _e224 = outToLightVector_arr[3i];
    outToLightVector3_ = _e224;
    return;
}

@vertex 
fn main(@location(3) inBoneIds: vec4<i32>, @location(4) inWeights: vec4<f32>, @location(0) inPosition: vec3<f32>, @location(2) inNormal: vec3<f32>, @location(1) inTextureCoord: vec2<f32>) -> VertexOutput {
    inBoneIds_1 = inBoneIds;
    inWeights_1 = inWeights;
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTextureCoord_1 = inTextureCoord;
    main_1();
    let _e24 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e24);
    let _e26 = outWorldPosition;
    let _e27 = outClipDistance;
    let _e28 = outViewPosition;
    let _e29 = unnamed.gl_Position;
    let _e30 = outTextureCoord;
    let _e31 = outNormal;
    let _e32 = outToCameraVector;
    let _e33 = outVisibility;
    let _e34 = outToLightVector0_;
    let _e35 = outToLightVector1_;
    let _e36 = outToLightVector2_;
    let _e37 = outToLightVector3_;
    return VertexOutput(_e26, _e27, _e28, _e29, _e30, _e31, _e32, _e33, _e34, _e35, _e36, _e37);
}
