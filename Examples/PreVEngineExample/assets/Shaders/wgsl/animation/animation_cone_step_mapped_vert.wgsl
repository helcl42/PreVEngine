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
    var mv3_: mat3x3<f32>;
    var TBN_1: mat3x3<f32>;
    var param_3: mat3x3<f32>;
    var param_4: vec3<f32>;
    var param_5: vec3<f32>;
    var param_6: vec3<f32>;
    var cameraPositionViewSpaceVec4_: vec4<f32>;
    var cameraPositionViewSpace: vec3<f32>;
    var i: i32;
    var light: Light;
    var lightPositionViewSpace: vec3<f32>;
    var outToLightVectorTangentSpace_arr: array<vec3<f32>, 4>;

    let _e64 = inBoneIds_1[0u];
    let _e67 = uboVS.bones[_e64];
    let _e69 = inWeights_1[0u];
    boneTransform = (_e67 * _e69);
    let _e72 = inBoneIds_1[1u];
    let _e75 = uboVS.bones[_e72];
    let _e77 = inWeights_1[1u];
    let _e78 = (_e75 * _e77);
    let _e79 = boneTransform;
    boneTransform = mat4x4<f32>((_e79[0] + _e78[0]), (_e79[1] + _e78[1]), (_e79[2] + _e78[2]), (_e79[3] + _e78[3]));
    let _e94 = inBoneIds_1[2u];
    let _e97 = uboVS.bones[_e94];
    let _e99 = inWeights_1[2u];
    let _e100 = (_e97 * _e99);
    let _e101 = boneTransform;
    boneTransform = mat4x4<f32>((_e101[0] + _e100[0]), (_e101[1] + _e100[1]), (_e101[2] + _e100[2]), (_e101[3] + _e100[3]));
    let _e116 = inBoneIds_1[3u];
    let _e119 = uboVS.bones[_e116];
    let _e121 = inWeights_1[3u];
    let _e122 = (_e119 * _e121);
    let _e123 = boneTransform;
    boneTransform = mat4x4<f32>((_e123[0] + _e122[0]), (_e123[1] + _e122[1]), (_e123[2] + _e122[2]), (_e123[3] + _e122[3]));
    let _e137 = boneTransform;
    let _e138 = inPosition_1;
    positionL = (_e137 * vec4<f32>(_e138.x, _e138.y, _e138.z, 1f));
    let _e144 = boneTransform;
    let _e145 = inNormal_1;
    normalL = (_e144 * vec4<f32>(_e145.x, _e145.y, _e145.z, 0f));
    let _e152 = uboVS.modelMatrix;
    let _e153 = positionL;
    let _e154 = _e153.xyz;
    worldPosition = (_e152 * vec4<f32>(_e154.x, _e154.y, _e154.z, 1f));
    let _e160 = worldPosition;
    outWorldPosition = _e160.xyz;
    let _e162 = worldPosition;
    let _e164 = uboVS.clipPlane;
    outClipDistance = dot(_e162, _e164);
    let _e168 = uboVS.viewMatrices[0i];
    let _e169 = worldPosition;
    viewPosition_1 = (_e168 * _e169);
    let _e171 = viewPosition_1;
    outViewPosition = _e171.xyz;
    let _e175 = uboVS.projectionMatrices[0i];
    let _e176 = viewPosition_1;
    unnamed.gl_Position = (_e175 * _e176);
    let _e179 = inTextureCoord_1;
    let _e181 = uboVS.textureNumberOfRows;
    let _e186 = uboVS.textureOffset;
    outTextureCoord = ((_e179 / vec2(f32(_e181))) + _e186.xy);
    let _e190 = uboVS.normalMatrix;
    let _e191 = normalL;
    let _e192 = _e191.xyz;
    outNormal = (_e190 * vec4<f32>(_e192.x, _e192.y, _e192.z, 0f)).xyz;
    let _e199 = viewPosition_1;
    param = _e199.xyz;
    let _e202 = uboVS.gradient;
    param_1 = _e202;
    let _e204 = uboVS.density;
    param_2 = _e204;
    let _e205 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e205;
    let _e208 = uboVS.viewMatrices[0i];
    let _e217 = uboVS.modelMatrix;
    mv3_ = (mat3x3<f32>(_e208[0].xyz, _e208[1].xyz, _e208[2].xyz) * mat3x3<f32>(_e217[0].xyz, _e217[1].xyz, _e217[2].xyz));
    let _e226 = mv3_;
    param_3 = _e226;
    let _e227 = inNormal_1;
    param_4 = _e227;
    let _e228 = inTangent_1;
    param_5 = _e228;
    let _e229 = inBiTangent_1;
    param_6 = _e229;
    let _e230 = CreateTBNMatrix_u0028_mf33_u003b_vf3_u003b_vf3_u003b_vf3_u003b((&param_3), (&param_4), (&param_5), (&param_6));
    TBN_1 = _e230;
    let _e233 = uboVS.viewMatrices[0i];
    let _e236 = uboVS.cameraPositions[0i];
    let _e237 = _e236.xyz;
    cameraPositionViewSpaceVec4_ = (_e233 * vec4<f32>(_e237.x, _e237.y, _e237.z, 1f));
    let _e243 = cameraPositionViewSpaceVec4_;
    let _e246 = cameraPositionViewSpaceVec4_[3u];
    cameraPositionViewSpace = (_e243.xyz / vec3(_e246));
    let _e249 = TBN_1;
    let _e250 = cameraPositionViewSpace;
    outToCameraVectorTangentSpace = (_e249 * _e250);
    let _e252 = TBN_1;
    let _e253 = viewPosition_1;
    outPositionTangentSpace = (_e252 * _e253.xyz);
    i = 0i;
    loop {
        let _e256 = i;
        let _e260 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e256) < _e260) {
            let _e262 = i;
            let _e266 = uboVS.lightning.lights[_e262];
            light.position = _e266.position;
            light.color = _e266.color;
            light.attenuation = _e266.attenuation;
            let _e275 = uboVS.viewMatrices[0i];
            let _e277 = light.position;
            let _e278 = _e277.xyz;
            lightPositionViewSpace = (_e275 * vec4<f32>(_e278.x, _e278.y, _e278.z, 1f)).xyz;
            let _e285 = i;
            let _e286 = TBN_1;
            let _e287 = lightPositionViewSpace;
            outToLightVectorTangentSpace_arr[_e285] = (_e286 * _e287);
            continue;
        } else {
            break;
        }
        continuing {
            let _e290 = i;
            i = (_e290 + 1i);
        }
    }
    let _e293 = outToLightVectorTangentSpace_arr[0i];
    outToLightVectorTangentSpace0_ = _e293;
    let _e295 = outToLightVectorTangentSpace_arr[1i];
    outToLightVectorTangentSpace1_ = _e295;
    let _e297 = outToLightVectorTangentSpace_arr[2i];
    outToLightVectorTangentSpace2_ = _e297;
    let _e299 = outToLightVectorTangentSpace_arr[3i];
    outToLightVectorTangentSpace3_ = _e299;
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
