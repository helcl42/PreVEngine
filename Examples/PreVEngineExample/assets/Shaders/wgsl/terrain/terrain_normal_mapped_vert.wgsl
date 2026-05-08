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

    let _e41 = (*transform);
    let _e42 = (*tangent);
    T = normalize((_e41 * _e42));
    let _e45 = (*transform);
    let _e46 = (*biTangent);
    B = normalize((_e45 * _e46));
    let _e49 = (*transform);
    let _e50 = (*normal);
    N = normalize((_e49 * _e50));
    let _e53 = T;
    let _e54 = B;
    let _e55 = N;
    TBN = transpose(mat3x3<f32>(vec3<f32>(_e53.x, _e53.y, _e53.z), vec3<f32>(_e54.x, _e54.y, _e54.z), vec3<f32>(_e55.x, _e55.y, _e55.z)));
    let _e70 = TBN;
    return _e70;
}

fn GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b(viewPosition: ptr<function, vec3<f32>>, gradient: ptr<function, f32>, density: ptr<function, f32>) -> f32 {
    var vertexToCameraDistance: f32;
    var visibility: f32;

    let _e38 = (*viewPosition);
    vertexToCameraDistance = length(_e38);
    let _e40 = vertexToCameraDistance;
    let _e41 = (*density);
    let _e43 = (*gradient);
    visibility = exp(-(pow((_e40 * _e41), _e43)));
    let _e47 = visibility;
    return clamp(_e47, 0f, 1f);
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

    let _e47 = uboVS.modelMatrix;
    let _e48 = inPosition_1;
    worldPosition = (_e47 * vec4<f32>(_e48.x, _e48.y, _e48.z, 1f));
    let _e54 = worldPosition;
    outWorldPosition = _e54.xyz;
    let _e56 = worldPosition;
    let _e58 = uboVS.clipPlane;
    outClipDistance = dot(_e56, _e58);
    let _e62 = uboVS.viewMatrices[0i];
    let _e63 = worldPosition;
    viewPosition_1 = (_e62 * _e63);
    let _e65 = viewPosition_1;
    outViewPosition = _e65.xyz;
    let _e69 = uboVS.projectionMatrices[0i];
    let _e70 = viewPosition_1;
    unnamed.gl_Position = (_e69 * _e70);
    let _e73 = inTextureCoord_1;
    outTextureCoord = _e73;
    let _e75 = uboVS.normalMatrix;
    let _e76 = inNormal_1;
    outNormal = (_e75 * vec4<f32>(_e76.x, _e76.y, _e76.z, 0f)).xyz;
    let _e83 = viewPosition_1;
    param = _e83.xyz;
    let _e86 = uboVS.gradient;
    param_1 = _e86;
    let _e88 = uboVS.density;
    param_2 = _e88;
    let _e89 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e89;
    let _e91 = uboVS.modelMatrix;
    param_3 = mat3x3<f32>(_e91[0].xyz, _e91[1].xyz, _e91[2].xyz);
    let _e99 = inNormal_1;
    param_4 = _e99;
    let _e100 = inTangent_1;
    param_5 = _e100;
    let _e101 = inBiTangent_1;
    param_6 = _e101;
    let _e102 = CreateTBNMatrix_u0028_mf33_u003b_vf3_u003b_vf3_u003b_vf3_u003b((&param_3), (&param_4), (&param_5), (&param_6));
    TBN_1 = _e102;
    let _e103 = TBN_1;
    let _e106 = uboVS.cameraPositions[0i];
    outToCameraVectorTangentSpace = (_e103 * _e106.xyz);
    let _e109 = TBN_1;
    let _e110 = worldPosition;
    outPositionTangentSpace = (_e109 * _e110.xyz);
    i = 0i;
    loop {
        let _e113 = i;
        let _e117 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e113) < _e117) {
            let _e119 = i;
            let _e123 = uboVS.lightning.lights[_e119];
            light.position = _e123.position;
            light.color = _e123.color;
            light.attenuation = _e123.attenuation;
            let _e130 = i;
            let _e131 = TBN_1;
            let _e133 = light.position;
            outToLightVectorTangentSpace_arr[_e130] = (_e131 * _e133.xyz);
            continue;
        } else {
            break;
        }
        continuing {
            let _e137 = i;
            i = (_e137 + 1i);
        }
    }
    let _e140 = outToLightVectorTangentSpace_arr[0i];
    outToLightVectorTangentSpace0_ = _e140;
    let _e142 = outToLightVectorTangentSpace_arr[1i];
    outToLightVectorTangentSpace1_ = _e142;
    let _e144 = outToLightVectorTangentSpace_arr[2i];
    outToLightVectorTangentSpace2_ = _e144;
    let _e146 = outToLightVectorTangentSpace_arr[3i];
    outToLightVectorTangentSpace3_ = _e146;
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
