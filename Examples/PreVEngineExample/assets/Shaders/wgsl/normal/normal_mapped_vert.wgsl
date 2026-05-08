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
var<private> inPosition_1: vec3<f32>;
var<private> outWorldPosition: vec3<f32>;
var<private> outClipDistance: f32;
var<private> outViewPosition: vec3<f32>;
var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> outTextureCoord: vec2<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> outNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
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

    let _e43 = (*transform);
    let _e44 = (*tangent);
    T = normalize((_e43 * _e44));
    let _e47 = (*transform);
    let _e48 = (*biTangent);
    B = normalize((_e47 * _e48));
    let _e51 = (*transform);
    let _e52 = (*normal);
    N = normalize((_e51 * _e52));
    let _e55 = T;
    let _e56 = B;
    let _e57 = N;
    TBN = transpose(mat3x3<f32>(vec3<f32>(_e55.x, _e55.y, _e55.z), vec3<f32>(_e56.x, _e56.y, _e56.z), vec3<f32>(_e57.x, _e57.y, _e57.z)));
    let _e72 = TBN;
    return _e72;
}

fn GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b(viewPosition: ptr<function, vec3<f32>>, gradient: ptr<function, f32>, density: ptr<function, f32>) -> f32 {
    var vertexToCameraDistance: f32;
    var visibility: f32;

    let _e40 = (*viewPosition);
    vertexToCameraDistance = length(_e40);
    let _e42 = vertexToCameraDistance;
    let _e43 = (*density);
    let _e45 = (*gradient);
    visibility = exp(-(pow((_e42 * _e43), _e45)));
    let _e49 = visibility;
    return clamp(_e49, 0f, 1f);
}

fn main_1() {
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

    let _e49 = uboVS.modelMatrix;
    let _e50 = inPosition_1;
    worldPosition = (_e49 * vec4<f32>(_e50.x, _e50.y, _e50.z, 1f));
    let _e56 = worldPosition;
    outWorldPosition = _e56.xyz;
    let _e58 = worldPosition;
    let _e60 = uboVS.clipPlane;
    outClipDistance = dot(_e58, _e60);
    let _e64 = uboVS.viewMatrices[0i];
    let _e65 = worldPosition;
    viewPosition_1 = (_e64 * _e65);
    let _e67 = viewPosition_1;
    outViewPosition = _e67.xyz;
    let _e71 = uboVS.projectionMatrices[0i];
    let _e72 = viewPosition_1;
    unnamed.gl_Position = (_e71 * _e72);
    let _e75 = inTextureCoord_1;
    let _e77 = uboVS.textureNumberOfRows;
    let _e82 = uboVS.textureOffset;
    outTextureCoord = ((_e75 / vec2(f32(_e77))) + _e82.xy);
    let _e86 = uboVS.normalMatrix;
    let _e87 = inNormal_1;
    outNormal = (_e86 * vec4<f32>(_e87.x, _e87.y, _e87.z, 0f)).xyz;
    let _e94 = viewPosition_1;
    param = _e94.xyz;
    let _e97 = uboVS.gradient;
    param_1 = _e97;
    let _e99 = uboVS.density;
    param_2 = _e99;
    let _e100 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e100;
    let _e102 = uboVS.modelMatrix;
    param_3 = mat3x3<f32>(_e102[0].xyz, _e102[1].xyz, _e102[2].xyz);
    let _e110 = inNormal_1;
    param_4 = _e110;
    let _e111 = inTangent_1;
    param_5 = _e111;
    let _e112 = inBiTangent_1;
    param_6 = _e112;
    let _e113 = CreateTBNMatrix_u0028_mf33_u003b_vf3_u003b_vf3_u003b_vf3_u003b((&param_3), (&param_4), (&param_5), (&param_6));
    TBN_1 = _e113;
    let _e114 = TBN_1;
    let _e117 = uboVS.cameraPositions[0i];
    outToCameraVectorTangentSpace = (_e114 * _e117.xyz);
    let _e120 = TBN_1;
    let _e121 = worldPosition;
    outPositionTangentSpace = (_e120 * _e121.xyz);
    i = 0i;
    loop {
        let _e124 = i;
        let _e128 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e124) < _e128) {
            let _e130 = i;
            let _e134 = uboVS.lightning.lights[_e130];
            light.position = _e134.position;
            light.color = _e134.color;
            light.attenuation = _e134.attenuation;
            let _e141 = i;
            let _e142 = TBN_1;
            let _e144 = light.position;
            outToLightVectorTangentSpace_arr[_e141] = (_e142 * _e144.xyz);
            continue;
        } else {
            break;
        }
        continuing {
            let _e148 = i;
            i = (_e148 + 1i);
        }
    }
    let _e151 = outToLightVectorTangentSpace_arr[0i];
    outToLightVectorTangentSpace0_ = _e151;
    let _e153 = outToLightVectorTangentSpace_arr[1i];
    outToLightVectorTangentSpace1_ = _e153;
    let _e155 = outToLightVectorTangentSpace_arr[2i];
    outToLightVectorTangentSpace2_ = _e155;
    let _e157 = outToLightVectorTangentSpace_arr[3i];
    outToLightVectorTangentSpace3_ = _e157;
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
    let _e32 = outNormal;
    let _e33 = outVisibility;
    let _e34 = outToCameraVectorTangentSpace;
    let _e35 = outPositionTangentSpace;
    let _e36 = outToLightVectorTangentSpace0_;
    let _e37 = outToLightVectorTangentSpace1_;
    let _e38 = outToLightVectorTangentSpace2_;
    let _e39 = outToLightVectorTangentSpace3_;
    return VertexOutput(_e27, _e28, _e29, _e30, _e31, _e32, _e33, _e34, _e35, _e36, _e37, _e38, _e39);
}
