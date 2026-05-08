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

    let _e45 = (*viewPosition);
    vertexToCameraDistance = length(_e45);
    let _e47 = vertexToCameraDistance;
    let _e48 = (*density);
    let _e50 = (*gradient);
    visibility = exp(-(pow((_e47 * _e48), _e50)));
    let _e54 = visibility;
    return clamp(_e54, 0f, 1f);
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

    let _e52 = inBoneIds_1[0u];
    let _e55 = uboVS.bones[_e52];
    let _e57 = inWeights_1[0u];
    boneTransform = (_e55 * _e57);
    let _e60 = inBoneIds_1[1u];
    let _e63 = uboVS.bones[_e60];
    let _e65 = inWeights_1[1u];
    let _e66 = (_e63 * _e65);
    let _e67 = boneTransform;
    boneTransform = mat4x4<f32>((_e67[0] + _e66[0]), (_e67[1] + _e66[1]), (_e67[2] + _e66[2]), (_e67[3] + _e66[3]));
    let _e82 = inBoneIds_1[2u];
    let _e85 = uboVS.bones[_e82];
    let _e87 = inWeights_1[2u];
    let _e88 = (_e85 * _e87);
    let _e89 = boneTransform;
    boneTransform = mat4x4<f32>((_e89[0] + _e88[0]), (_e89[1] + _e88[1]), (_e89[2] + _e88[2]), (_e89[3] + _e88[3]));
    let _e104 = inBoneIds_1[3u];
    let _e107 = uboVS.bones[_e104];
    let _e109 = inWeights_1[3u];
    let _e110 = (_e107 * _e109);
    let _e111 = boneTransform;
    boneTransform = mat4x4<f32>((_e111[0] + _e110[0]), (_e111[1] + _e110[1]), (_e111[2] + _e110[2]), (_e111[3] + _e110[3]));
    let _e125 = boneTransform;
    let _e126 = inPosition_1;
    positionL = (_e125 * vec4<f32>(_e126.x, _e126.y, _e126.z, 1f));
    let _e132 = boneTransform;
    let _e133 = inNormal_1;
    normalL = (_e132 * vec4<f32>(_e133.x, _e133.y, _e133.z, 0f));
    let _e140 = uboVS.modelMatrix;
    let _e141 = positionL;
    let _e142 = _e141.xyz;
    worldPosition = (_e140 * vec4<f32>(_e142.x, _e142.y, _e142.z, 1f));
    let _e148 = worldPosition;
    outWorldPosition = _e148.xyz;
    let _e150 = worldPosition;
    let _e152 = uboVS.clipPlane;
    outClipDistance = dot(_e150, _e152);
    let _e156 = uboVS.viewMatrices[0i];
    let _e157 = worldPosition;
    viewPosition_1 = (_e156 * _e157);
    let _e159 = viewPosition_1;
    outViewPosition = _e159.xyz;
    let _e163 = uboVS.projectionMatrices[0i];
    let _e164 = viewPosition_1;
    unnamed.gl_Position = (_e163 * _e164);
    let _e167 = inTextureCoord_1;
    let _e169 = uboVS.textureNumberOfRows;
    let _e174 = uboVS.textureOffset;
    outTextureCoord = ((_e167 / vec2(f32(_e169))) + _e174.xy);
    let _e178 = uboVS.normalMatrix;
    let _e179 = normalL;
    let _e180 = _e179.xyz;
    outNormal = (_e178 * vec4<f32>(_e180.x, _e180.y, _e180.z, 0f)).xyz;
    i = 0i;
    loop {
        let _e187 = i;
        let _e191 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e187) < _e191) {
            let _e193 = i;
            let _e197 = uboVS.lightning.lights[_e193];
            light.position = _e197.position;
            light.color = _e197.color;
            light.attenuation = _e197.attenuation;
            let _e204 = i;
            let _e206 = light.position;
            let _e208 = worldPosition;
            outToLightVector_arr[_e204] = (_e206.xyz - _e208.xyz);
            continue;
        } else {
            break;
        }
        continuing {
            let _e212 = i;
            i = (_e212 + 1i);
        }
    }
    let _e216 = uboVS.cameraPositions[0i];
    let _e218 = worldPosition;
    outToCameraVector = (_e216.xyz - _e218.xyz);
    let _e221 = viewPosition_1;
    param = _e221.xyz;
    let _e224 = uboVS.gradient;
    param_1 = _e224;
    let _e226 = uboVS.density;
    param_2 = _e226;
    let _e227 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e227;
    let _e229 = outToLightVector_arr[0i];
    outToLightVector0_ = _e229;
    let _e231 = outToLightVector_arr[1i];
    outToLightVector1_ = _e231;
    let _e233 = outToLightVector_arr[2i];
    outToLightVector2_ = _e233;
    let _e235 = outToLightVector_arr[3i];
    outToLightVector3_ = _e235;
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
