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

    let _e37 = (*viewPosition);
    vertexToCameraDistance = length(_e37);
    let _e39 = vertexToCameraDistance;
    let _e40 = (*density);
    let _e42 = (*gradient);
    visibility = exp(-(pow((_e39 * _e40), _e42)));
    let _e46 = visibility;
    return clamp(_e46, 0f, 1f);
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

    let _e41 = uboVS.modelMatrix;
    let _e42 = inPosition_1;
    worldPosition = (_e41 * vec4<f32>(_e42.x, _e42.y, _e42.z, 1f));
    let _e48 = worldPosition;
    outWorldPosition = _e48.xyz;
    let _e50 = worldPosition;
    let _e52 = uboVS.clipPlane;
    outClipDistance = dot(_e50, _e52);
    let _e56 = uboVS.viewMatrices[0i];
    let _e57 = worldPosition;
    viewPosition_1 = (_e56 * _e57);
    let _e59 = viewPosition_1;
    outViewPosition = _e59.xyz;
    let _e63 = uboVS.projectionMatrices[0i];
    let _e64 = viewPosition_1;
    unnamed.gl_Position = (_e63 * _e64);
    let _e67 = inTextureCoord_1;
    let _e69 = uboVS.textureNumberOfRows;
    let _e74 = uboVS.textureOffset;
    outTextureCoord = ((_e67 / vec2(f32(_e69))) + _e74.xy);
    let _e78 = uboVS.normalMatrix;
    let _e79 = inNormal_1;
    outNormal = (_e78 * vec4<f32>(_e79.x, _e79.y, _e79.z, 0f)).xyz;
    i = 0i;
    loop {
        let _e86 = i;
        let _e90 = uboVS.lightning.realCountOfLights;
        if (bitcast<u32>(_e86) < _e90) {
            let _e92 = i;
            let _e96 = uboVS.lightning.lights[_e92];
            light.position = _e96.position;
            light.color = _e96.color;
            light.attenuation = _e96.attenuation;
            let _e103 = i;
            let _e105 = light.position;
            let _e107 = worldPosition;
            outToLightVector_arr[_e103] = (_e105.xyz - _e107.xyz);
            continue;
        } else {
            break;
        }
        continuing {
            let _e111 = i;
            i = (_e111 + 1i);
        }
    }
    let _e115 = uboVS.cameraPositions[0i];
    let _e117 = worldPosition;
    outToCameraVector = (_e115.xyz - _e117.xyz);
    let _e120 = viewPosition_1;
    param = _e120.xyz;
    let _e123 = uboVS.gradient;
    param_1 = _e123;
    let _e125 = uboVS.density;
    param_2 = _e125;
    let _e126 = GetVisibility_u0028_vf3_u003b_f1_u003b_f1_u003b((&param), (&param_1), (&param_2));
    outVisibility = _e126;
    let _e128 = outToLightVector_arr[0i];
    outToLightVector0_ = _e128;
    let _e130 = outToLightVector_arr[1i];
    outToLightVector1_ = _e130;
    let _e132 = outToLightVector_arr[2i];
    outToLightVector2_ = _e132;
    let _e134 = outToLightVector_arr[3i];
    outToLightVector3_ = _e134;
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
