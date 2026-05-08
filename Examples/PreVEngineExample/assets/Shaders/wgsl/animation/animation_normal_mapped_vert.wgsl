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
    textureOffset: vec4<f32>,
    textureNumberOfRows: u32,
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
    @location(11) member_1: f32,
    @location(3) member_2: vec3<f32>,
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member_3: vec2<f32>,
    @location(1) member_4: vec3<f32>,
    @location(4) member_5: f32,
    @location(5) member_6: vec3<f32>,
    @location(6) member_7: vec3<f32>,
    @location(7) member_8: vec3<f32>,
    @location(8) member_9: vec3<f32>,
    @location(9) member_10: vec3<f32>,
    @location(10) member_11: vec3<f32>,
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
var<private> outVisibility: f32;
var<private> inTangent_1: vec3<f32>;
var<private> inBiTangent_1: vec3<f32>;
var<private> outToCameraVectorTangentSpace: vec3<f32>;
var<private> outPositionTangentSpace: vec3<f32>;
var<private> outToLightVectorTangentSpace0_: vec3<f32>;
var<private> outToLightVectorTangentSpace1_: vec3<f32>;
var<private> outToLightVectorTangentSpace2_: vec3<f32>;
var<private> outToLightVectorTangentSpace3_: vec3<f32>;

fn CreateTBNMatrix_u0028_mf33_u003b_vf3_u003b_vf3_u003b_vf3_u003b(transform: ptr<function, mat3x3<f32>>, normal: ptr<function, vec3<f32>>, tangent: ptr<function, vec3<f32>>, biTangent: ptr<function, vec3<f32>>) -> mat3x3<f32> {
    var T: vec3<f32>;
    var B: vec3<f32>;
    var N: vec3<f32>;
    var TBN: mat3x3<f32>;

    let _e51 = (*transform);
    let _e52 = (*tangent);
    T = normalize((_e51 * _e52));
    let _e55 = (*transform);
    let _e56 = (*biTangent);
    B = normalize((_e55 * _e56));
    let _e59 = (*transform);
    let _e60 = (*normal);
    N = normalize((_e59 * _e60));
    let _e63 = T;
    let _e64 = B;
    let _e65 = N;
    TBN = transpose(mat3x3<f32>(vec3<f32>(_e63.x, _e63.y, _e63.z), vec3<f32>(_e64.x, _e64.y, _e64.z), vec3<f32>(_e65.x, _e65.y, _e65.z)));
    let _e80 = TBN;
    return _e80;
}

fn GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b(viewPosition: ptr<function, vec3<f32>>, gradient: ptr<function, f32>, density: ptr<function, f32>) -> f32 {
    var vertexToCameraDistance: f32;
    var visibility: f32;

    let _e48 = (*viewPosition);
    vertexToCameraDistance = length(_e48);
    let _e50 = vertexToCameraDistance;
    let _e51 = (*density);
    let _e53 = (*gradient);
    visibility = exp(-(pow((_e50 * _e51), _e53)));
    let _e57 = visibility;
    return clamp(_e57, 0f, 1f);
}

fn main_1() {
    var boneTransform: mat4x4<f32>;
    var positionL: vec4<f32>;
    var normalL: vec4<f32>;
    var worldPosition: vec4<f32>;
    var viewPosition_1: vec4<f32>;
    var param: vec3<f32>;
    var param_1: f32;
    var param_2: f32;
    var TBN_1: mat3x3<f32>;
    var param_3: mat3x3<f32>;
    var param_4: vec3<f32>;
    var param_5: vec3<f32>;
    var param_6: vec3<f32>;
    var i: i32;
    var light: Light;
    var outToLightVectorTangentSpace_arr: array<vec3<f32>, 4>;

    let _e60 = inBoneIds_1[0u];
    let _e63 = uboVS.bones[_e60];
    let _e65 = inWeights_1[0u];
    boneTransform = (_e63 * _e65);
    let _e68 = inBoneIds_1[1u];
    let _e71 = uboVS.bones[_e68];
    let _e73 = inWeights_1[1u];
    let _e74 = (_e71 * _e73);
    let _e75 = boneTransform;
    boneTransform = mat4x4<f32>((_e75[0] + _e74[0]), (_e75[1] + _e74[1]), (_e75[2] + _e74[2]), (_e75[3] + _e74[3]));
    let _e90 = inBoneIds_1[2u];
    let _e93 = uboVS.bones[_e90];
    let _e95 = inWeights_1[2u];
    let _e96 = (_e93 * _e95);
    let _e97 = boneTransform;
    boneTransform = mat4x4<f32>((_e97[0] + _e96[0]), (_e97[1] + _e96[1]), (_e97[2] + _e96[2]), (_e97[3] + _e96[3]));
    let _e112 = inBoneIds_1[3u];
    let _e115 = uboVS.bones[_e112];
    let _e117 = inWeights_1[3u];
    let _e118 = (_e115 * _e117);
    let _e119 = boneTransform;
    boneTransform = mat4x4<f32>((_e119[0] + _e118[0]), (_e119[1] + _e118[1]), (_e119[2] + _e118[2]), (_e119[3] + _e118[3]));
    let _e133 = boneTransform;
    let _e134 = inPosition_1;
    positionL = (_e133 * vec4<f32>(_e134.x, _e134.y, _e134.z, 1f));
    let _e140 = boneTransform;
    let _e141 = inNormal_1;
    normalL = (_e140 * vec4<f32>(_e141.x, _e141.y, _e141.z, 0f));
    let _e148 = uboVS.modelMatrix;
    let _e149 = positionL;
    let _e150 = _e149.xyz;
    worldPosition = (_e148 * vec4<f32>(_e150.x, _e150.y, _e150.z, 1f));
    let _e156 = worldPosition;
    outWorldPosition = _e156.xyz;
    let _e158 = worldPosition;
    let _e160 = uboVS.clipPlane;
    outClipDistance = dot(_e158, _e160);
    let _e164 = uboVS.viewMatrices[0i];
    let _e165 = worldPosition;
    viewPosition_1 = (_e164 * _e165);
    let _e167 = viewPosition_1;
    outViewPosition = _e167.xyz;
    let _e171 = uboVS.projectionMatrices[0i];
    let _e172 = viewPosition_1;
    unnamed.gl_Position = (_e171 * _e172);
    let _e175 = inTextureCoord_1;
    let _e177 = uboVS.textureNumberOfRows;
    let _e182 = uboVS.textureOffset;
    outTextureCoord = ((_e175 / vec2(f32(_e177))) + _e182.xy);
    let _e186 = uboVS.normalMatrix;
    let _e187 = normalL;
    let _e188 = _e187.xyz;
    outNormal = (_e186 * vec4<f32>(_e188.x, _e188.y, _e188.z, 0f)).xyz;
    let _e195 = viewPosition_1;
    param = _e195.xyz;
    let _e198 = uboVS.gradient;
    param_1 = _e198;
    let _e200 = uboVS.density;
    param_2 = _e200;
    let _e201 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e201;
    let _e203 = uboVS.modelMatrix;
    param_3 = mat3x3<f32>(_e203[0].xyz, _e203[1].xyz, _e203[2].xyz);
    let _e211 = inNormal_1;
    param_4 = _e211;
    let _e212 = inTangent_1;
    param_5 = _e212;
    let _e213 = inBiTangent_1;
    param_6 = _e213;
    let _e214 = CreateTBNMatrix_u0028_mf33_u003b_vf3_u003b_vf3_u003b_vf3_u003b((&param_3), (&param_4), (&param_5), (&param_6));
    TBN_1 = _e214;
    let _e215 = TBN_1;
    let _e218 = uboVS.cameraPositions[0i];
    outToCameraVectorTangentSpace = (_e215 * _e218.xyz);
    let _e221 = TBN_1;
    let _e222 = worldPosition;
    outPositionTangentSpace = (_e221 * _e222.xyz);
    i = 0i;
    loop {
        let _e225 = i;
        let _e229 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e225) < _e229) {
            let _e231 = i;
            let _e235 = uboVS.lightning.lights[_e231];
            light.position = _e235.position;
            light.color = _e235.color;
            light.attenuation = _e235.attenuation;
            let _e242 = i;
            let _e243 = TBN_1;
            let _e245 = light.position;
            outToLightVectorTangentSpace_arr[_e242] = (_e243 * _e245.xyz);
            continue;
        } else {
            break;
        }
        continuing {
            let _e249 = i;
            i = (_e249 + 1i);
        }
    }
    let _e252 = outToLightVectorTangentSpace_arr[0i];
    outToLightVectorTangentSpace0_ = _e252;
    let _e254 = outToLightVectorTangentSpace_arr[1i];
    outToLightVectorTangentSpace1_ = _e254;
    let _e256 = outToLightVectorTangentSpace_arr[2i];
    outToLightVectorTangentSpace2_ = _e256;
    let _e258 = outToLightVectorTangentSpace_arr[3i];
    outToLightVectorTangentSpace3_ = _e258;
    return;
}

@vertex 
fn main(@location(3) inBoneIds: vec4<i32>, @location(4) inWeights: vec4<f32>, @location(0) inPosition: vec3<f32>, @location(2) inNormal: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(5) inTangent: vec3<f32>, @location(6) inBiTangent: vec3<f32>) -> VertexOutput {
    inBoneIds_1 = inBoneIds;
    inWeights_1 = inWeights;
    inPosition_1 = inPosition;
    inNormal_1 = inNormal;
    inTextureCoord_1 = inTextureCoord;
    inTangent_1 = inTangent;
    inBiTangent_1 = inBiTangent;
    main_1();
    let _e29 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e29);
    let _e31 = outWorldPosition;
    let _e32 = outClipDistance;
    let _e33 = outViewPosition;
    let _e34 = unnamed.gl_Position;
    let _e35 = outTextureCoord;
    let _e36 = outNormal;
    let _e37 = outVisibility;
    let _e38 = outToCameraVectorTangentSpace;
    let _e39 = outPositionTangentSpace;
    let _e40 = outToLightVectorTangentSpace0_;
    let _e41 = outToLightVectorTangentSpace1_;
    let _e42 = outToLightVectorTangentSpace2_;
    let _e43 = outToLightVectorTangentSpace3_;
    return VertexOutput(_e31, _e32, _e33, _e34, _e35, _e36, _e37, _e38, _e39, _e40, _e41, _e42, _e43);
}
