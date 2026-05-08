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
var<private> inPosition_1: vec3<f32>;
var<private> outWorldPosition: vec3<f32>;
var<private> outClipDistance: f32;
var<private> outViewPosition: vec3<f32>;
var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> outTextureCoord: vec2<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> outNormal: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> outToCameraVector: vec3<f32>;
var<private> outVisibility: f32;
var<private> outToLightVector0_: vec3<f32>;
var<private> outToLightVector1_: vec3<f32>;
var<private> outToLightVector2_: vec3<f32>;
var<private> outToLightVector3_: vec3<f32>;

fn GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b(viewPosition: ptr<function, vec3<f32>>, gradient: ptr<function, f32>, density: ptr<function, f32>) -> f32 {
    var vertexToCameraDistance: f32;
    var visibility: f32;

    let _e35 = (*viewPosition);
    vertexToCameraDistance = length(_e35);
    let _e37 = vertexToCameraDistance;
    let _e38 = (*density);
    let _e40 = (*gradient);
    visibility = exp(-(pow((_e37 * _e38), _e40)));
    let _e44 = visibility;
    return clamp(_e44, 0f, 1f);
}

fn main_1() {
    var worldPosition: vec4<f32>;
    var viewPosition_1: vec4<f32>;
    var i: i32;
    var light: Light;
    var outToLightVector_arr: array<vec3<f32>, 4>;
    var param: vec3<f32>;
    var param_1: f32;
    var param_2: f32;

    let _e39 = uboVS.modelMatrix;
    let _e40 = inPosition_1;
    worldPosition = (_e39 * vec4<f32>(_e40.x, _e40.y, _e40.z, 1f));
    let _e46 = worldPosition;
    outWorldPosition = _e46.xyz;
    let _e48 = worldPosition;
    let _e50 = uboVS.clipPlane;
    outClipDistance = dot(_e48, _e50);
    let _e54 = uboVS.viewMatrices[0i];
    let _e55 = worldPosition;
    viewPosition_1 = (_e54 * _e55);
    let _e57 = viewPosition_1;
    outViewPosition = _e57.xyz;
    let _e61 = uboVS.projectionMatrices[0i];
    let _e62 = viewPosition_1;
    unnamed.gl_Position = (_e61 * _e62);
    let _e65 = inTextureCoord_1;
    outTextureCoord = _e65;
    let _e67 = uboVS.normalMatrix;
    let _e68 = inNormal_1;
    outNormal = (_e67 * vec4<f32>(_e68.x, _e68.y, _e68.z, 0f)).xyz;
    i = 0i;
    loop {
        let _e75 = i;
        let _e79 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e75) < _e79) {
            let _e81 = i;
            let _e85 = uboVS.lightning.lights[_e81];
            light.position = _e85.position;
            light.color = _e85.color;
            light.attenuation = _e85.attenuation;
            let _e92 = i;
            let _e94 = light.position;
            let _e96 = worldPosition;
            outToLightVector_arr[_e92] = (_e94.xyz - _e96.xyz);
            continue;
        } else {
            break;
        }
        continuing {
            let _e100 = i;
            i = (_e100 + 1i);
        }
    }
    let _e104 = uboVS.cameraPositions[0i];
    let _e106 = worldPosition;
    outToCameraVector = (_e104.xyz - _e106.xyz);
    let _e109 = viewPosition_1;
    param = _e109.xyz;
    let _e112 = uboVS.gradient;
    param_1 = _e112;
    let _e114 = uboVS.density;
    param_2 = _e114;
    let _e115 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e115;
    let _e117 = outToLightVector_arr[0i];
    outToLightVector0_ = _e117;
    let _e119 = outToLightVector_arr[1i];
    outToLightVector1_ = _e119;
    let _e121 = outToLightVector_arr[2i];
    outToLightVector2_ = _e121;
    let _e123 = outToLightVector_arr[3i];
    outToLightVector3_ = _e123;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    main_1();
    let _e20 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e20);
    let _e22 = outWorldPosition;
    let _e23 = outClipDistance;
    let _e24 = outViewPosition;
    let _e25 = unnamed.gl_Position;
    let _e26 = outTextureCoord;
    let _e27 = outNormal;
    let _e28 = outToCameraVector;
    let _e29 = outVisibility;
    let _e30 = outToLightVector0_;
    let _e31 = outToLightVector1_;
    let _e32 = outToLightVector2_;
    let _e33 = outToLightVector3_;
    return VertexOutput(_e22, _e23, _e24, _e25, _e26, _e27, _e28, _e29, _e30, _e31, _e32, _e33);
}
