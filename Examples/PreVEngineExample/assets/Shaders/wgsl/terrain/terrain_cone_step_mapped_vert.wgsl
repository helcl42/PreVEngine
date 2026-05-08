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
    @location(11) member_1: f32,
    @location(3) member_2: vec3<f32>,
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member_3: vec2<f32>,
    @location(4) member_4: f32,
    @location(1) member_5: vec3<f32>,
    @location(5) member_6: vec3<f32>,
    @location(6) member_7: vec3<f32>,
    @location(7) member_8: vec3<f32>,
    @location(8) member_9: vec3<f32>,
    @location(9) member_10: vec3<f32>,
    @location(10) member_11: vec3<f32>,
}

@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> inPosition_1: vec3<f32>;
var<private> outWorldPosition: vec3<f32>;
var<private> outClipDistance: f32;
var<private> outViewPosition: vec3<f32>;
var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> outTextureCoord: vec2<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> outVisibility: f32;
var<private> outNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
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

    let _e42 = (*transform);
    let _e43 = (*tangent);
    T = normalize((_e42 * _e43));
    let _e46 = (*transform);
    let _e47 = (*biTangent);
    B = normalize((_e46 * _e47));
    let _e50 = (*transform);
    let _e51 = (*normal);
    N = normalize((_e50 * _e51));
    let _e54 = T;
    let _e55 = B;
    let _e56 = N;
    TBN = transpose(mat3x3<f32>(vec3<f32>(_e54.x, _e54.y, _e54.z), vec3<f32>(_e55.x, _e55.y, _e55.z), vec3<f32>(_e56.x, _e56.y, _e56.z)));
    let _e71 = TBN;
    return _e71;
}

fn GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b(viewPosition: ptr<function, vec3<f32>>, gradient: ptr<function, f32>, density: ptr<function, f32>) -> f32 {
    var vertexToCameraDistance: f32;
    var visibility: f32;

    let _e39 = (*viewPosition);
    vertexToCameraDistance = length(_e39);
    let _e41 = vertexToCameraDistance;
    let _e42 = (*density);
    let _e44 = (*gradient);
    visibility = exp(-(pow((_e41 * _e42), _e44)));
    let _e48 = visibility;
    return clamp(_e48, 0f, 1f);
}

fn main_1() {
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

    let _e52 = uboVS.modelMatrix;
    let _e53 = inPosition_1;
    worldPosition = (_e52 * vec4<f32>(_e53.x, _e53.y, _e53.z, 1f));
    let _e59 = worldPosition;
    outWorldPosition = _e59.xyz;
    let _e61 = worldPosition;
    let _e63 = uboVS.clipPlane;
    outClipDistance = dot(_e61, _e63);
    let _e67 = uboVS.viewMatrices[0i];
    let _e68 = worldPosition;
    viewPosition_1 = (_e67 * _e68);
    let _e70 = viewPosition_1;
    outViewPosition = _e70.xyz;
    let _e74 = uboVS.projectionMatrices[0i];
    let _e75 = viewPosition_1;
    unnamed.gl_Position = (_e74 * _e75);
    let _e78 = inTextureCoord_1;
    outTextureCoord = _e78;
    let _e79 = viewPosition_1;
    param = _e79.xyz;
    let _e82 = uboVS.gradient;
    param_1 = _e82;
    let _e84 = uboVS.density;
    param_2 = _e84;
    let _e85 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e85;
    let _e87 = uboVS.normalMatrix;
    let _e88 = inNormal_1;
    outNormal = (_e87 * vec4<f32>(_e88.x, _e88.y, _e88.z, 0f)).xyz;
    let _e97 = uboVS.viewMatrices[0i];
    let _e106 = uboVS.modelMatrix;
    mv3_ = (mat3x3<f32>(_e97[0].xyz, _e97[1].xyz, _e97[2].xyz) * mat3x3<f32>(_e106[0].xyz, _e106[1].xyz, _e106[2].xyz));
    let _e115 = mv3_;
    param_3 = _e115;
    let _e116 = inNormal_1;
    param_4 = _e116;
    let _e117 = inTangent_1;
    param_5 = _e117;
    let _e118 = inBiTangent_1;
    param_6 = _e118;
    let _e119 = CreateTBNMatrix_u0028_mf33_u003b_vf3_u003b_vf3_u003b_vf3_u003b((&param_3), (&param_4), (&param_5), (&param_6));
    TBN_1 = _e119;
    let _e122 = uboVS.viewMatrices[0i];
    let _e125 = uboVS.cameraPositions[0i];
    let _e126 = _e125.xyz;
    cameraPositionViewSpaceVec4_ = (_e122 * vec4<f32>(_e126.x, _e126.y, _e126.z, 1f));
    let _e132 = cameraPositionViewSpaceVec4_;
    let _e135 = cameraPositionViewSpaceVec4_[3u];
    cameraPositionViewSpace = (_e132.xyz / vec3(_e135));
    let _e138 = TBN_1;
    let _e139 = cameraPositionViewSpace;
    outToCameraVectorTangentSpace = (_e138 * _e139);
    let _e141 = TBN_1;
    let _e142 = viewPosition_1;
    outPositionTangentSpace = (_e141 * _e142.xyz);
    i = 0i;
    loop {
        let _e145 = i;
        let _e149 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e145) < _e149) {
            let _e151 = i;
            let _e155 = uboVS.lightning.lights[_e151];
            light.position = _e155.position;
            light.color = _e155.color;
            light.attenuation = _e155.attenuation;
            let _e164 = uboVS.viewMatrices[0i];
            let _e166 = light.position;
            let _e167 = _e166.xyz;
            lightPositionViewSpace = (_e164 * vec4<f32>(_e167.x, _e167.y, _e167.z, 1f)).xyz;
            let _e174 = i;
            let _e175 = TBN_1;
            let _e176 = lightPositionViewSpace;
            outToLightVectorTangentSpace_arr[_e174] = (_e175 * _e176);
            continue;
        } else {
            break;
        }
        continuing {
            let _e179 = i;
            i = (_e179 + 1i);
        }
    }
    let _e182 = outToLightVectorTangentSpace_arr[0i];
    outToLightVectorTangentSpace0_ = _e182;
    let _e184 = outToLightVectorTangentSpace_arr[1i];
    outToLightVectorTangentSpace1_ = _e184;
    let _e186 = outToLightVectorTangentSpace_arr[2i];
    outToLightVectorTangentSpace2_ = _e186;
    let _e188 = outToLightVectorTangentSpace_arr[3i];
    outToLightVectorTangentSpace3_ = _e188;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>, @location(3) inTangent: vec3<f32>, @location(4) inBiTangent: vec3<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    inBiTangent_1 = inBiTangent;
    main_1();
    let _e25 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e25);
    let _e27 = outWorldPosition;
    let _e28 = outClipDistance;
    let _e29 = outViewPosition;
    let _e30 = unnamed.gl_Position;
    let _e31 = outTextureCoord;
    let _e32 = outVisibility;
    let _e33 = outNormal;
    let _e34 = outToCameraVectorTangentSpace;
    let _e35 = outPositionTangentSpace;
    let _e36 = outToLightVectorTangentSpace0_;
    let _e37 = outToLightVectorTangentSpace1_;
    let _e38 = outToLightVectorTangentSpace2_;
    let _e39 = outToLightVectorTangentSpace3_;
    return VertexOutput(_e27, _e28, _e29, _e30, _e31, _e32, _e33, _e34, _e35, _e36, _e37, _e38, _e39);
}
