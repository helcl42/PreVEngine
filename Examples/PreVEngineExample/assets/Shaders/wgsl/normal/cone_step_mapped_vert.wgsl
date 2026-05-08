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

    let _e44 = (*transform);
    let _e45 = (*tangent);
    T = normalize((_e44 * _e45));
    let _e48 = (*transform);
    let _e49 = (*biTangent);
    B = normalize((_e48 * _e49));
    let _e52 = (*transform);
    let _e53 = (*normal);
    N = normalize((_e52 * _e53));
    let _e56 = T;
    let _e57 = B;
    let _e58 = N;
    TBN = transpose(mat3x3<f32>(vec3<f32>(_e56.x, _e56.y, _e56.z), vec3<f32>(_e57.x, _e57.y, _e57.z), vec3<f32>(_e58.x, _e58.y, _e58.z)));
    let _e73 = TBN;
    return _e73;
}

fn GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b(viewPosition: ptr<function, vec3<f32>>, gradient: ptr<function, f32>, density: ptr<function, f32>) -> f32 {
    var vertexToCameraDistance: f32;
    var visibility: f32;

    let _e41 = (*viewPosition);
    vertexToCameraDistance = length(_e41);
    let _e43 = vertexToCameraDistance;
    let _e44 = (*density);
    let _e46 = (*gradient);
    visibility = exp(-(pow((_e43 * _e44), _e46)));
    let _e50 = visibility;
    return clamp(_e50, 0f, 1f);
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

    let _e54 = uboVS.modelMatrix;
    let _e55 = inPosition_1;
    worldPosition = (_e54 * vec4<f32>(_e55.x, _e55.y, _e55.z, 1f));
    let _e61 = worldPosition;
    outWorldPosition = _e61.xyz;
    let _e63 = worldPosition;
    let _e65 = uboVS.clipPlane;
    outClipDistance = dot(_e63, _e65);
    let _e69 = uboVS.viewMatrices[0i];
    let _e70 = worldPosition;
    viewPosition_1 = (_e69 * _e70);
    let _e72 = viewPosition_1;
    outViewPosition = _e72.xyz;
    let _e76 = uboVS.projectionMatrices[0i];
    let _e77 = viewPosition_1;
    unnamed.gl_Position = (_e76 * _e77);
    let _e80 = inTextureCoord_1;
    let _e82 = uboVS.textureNumberOfRows;
    let _e87 = uboVS.textureOffset;
    outTextureCoord = ((_e80 / vec2(f32(_e82))) + _e87.xy);
    let _e91 = uboVS.normalMatrix;
    let _e92 = inNormal_1;
    outNormal = (_e91 * vec4<f32>(_e92.x, _e92.y, _e92.z, 0f)).xyz;
    let _e99 = viewPosition_1;
    param = _e99.xyz;
    let _e102 = uboVS.gradient;
    param_1 = _e102;
    let _e104 = uboVS.density;
    param_2 = _e104;
    let _e105 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e105;
    let _e108 = uboVS.viewMatrices[0i];
    let _e117 = uboVS.modelMatrix;
    mv3_ = (mat3x3<f32>(_e108[0].xyz, _e108[1].xyz, _e108[2].xyz) * mat3x3<f32>(_e117[0].xyz, _e117[1].xyz, _e117[2].xyz));
    let _e126 = mv3_;
    param_3 = _e126;
    let _e127 = inNormal_1;
    param_4 = _e127;
    let _e128 = inTangent_1;
    param_5 = _e128;
    let _e129 = inBiTangent_1;
    param_6 = _e129;
    let _e130 = CreateTBNMatrix_u0028_mf33_u003b_vf3_u003b_vf3_u003b_vf3_u003b((&param_3), (&param_4), (&param_5), (&param_6));
    TBN_1 = _e130;
    let _e133 = uboVS.viewMatrices[0i];
    let _e136 = uboVS.cameraPositions[0i];
    let _e137 = _e136.xyz;
    cameraPositionViewSpaceVec4_ = (_e133 * vec4<f32>(_e137.x, _e137.y, _e137.z, 1f));
    let _e143 = cameraPositionViewSpaceVec4_;
    let _e146 = cameraPositionViewSpaceVec4_[3u];
    cameraPositionViewSpace = (_e143.xyz / vec3(_e146));
    let _e149 = TBN_1;
    let _e150 = cameraPositionViewSpace;
    outToCameraVectorTangentSpace = (_e149 * _e150);
    let _e152 = TBN_1;
    let _e153 = viewPosition_1;
    outPositionTangentSpace = (_e152 * _e153.xyz);
    i = 0i;
    loop {
        let _e156 = i;
        let _e160 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e156) < _e160) {
            let _e162 = i;
            let _e166 = uboVS.lightning.lights[_e162];
            light.position = _e166.position;
            light.color = _e166.color;
            light.attenuation = _e166.attenuation;
            let _e175 = uboVS.viewMatrices[0i];
            let _e177 = light.position;
            let _e178 = _e177.xyz;
            lightPositionViewSpace = (_e175 * vec4<f32>(_e178.x, _e178.y, _e178.z, 1f)).xyz;
            let _e185 = i;
            let _e186 = TBN_1;
            let _e187 = lightPositionViewSpace;
            outToLightVectorTangentSpace_arr[_e185] = (_e186 * _e187);
            continue;
        } else {
            break;
        }
        continuing {
            let _e190 = i;
            i = (_e190 + 1i);
        }
    }
    let _e193 = outToLightVectorTangentSpace_arr[0i];
    outToLightVectorTangentSpace0_ = _e193;
    let _e195 = outToLightVectorTangentSpace_arr[1i];
    outToLightVectorTangentSpace1_ = _e195;
    let _e197 = outToLightVectorTangentSpace_arr[2i];
    outToLightVectorTangentSpace2_ = _e197;
    let _e199 = outToLightVectorTangentSpace_arr[3i];
    outToLightVectorTangentSpace3_ = _e199;
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
