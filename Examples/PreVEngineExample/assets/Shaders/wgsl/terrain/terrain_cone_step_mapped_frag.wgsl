struct ShadowsCascade {
    viewProjectionMatrix: mat4x4<f32>,
    split: vec4<f32>,
}

struct Shadows {
    cascades: array<ShadowsCascade, 4>,
    enabled: u32,
    useReverseDepth: u32,
}

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

struct Material {
    color: vec4<f32>,
    shineDamper: f32,
    reflectivity: f32,
}

struct UniformBufferObject {
    shadows: Shadows,
    lightning: Lightning,
    material: array<Material, 4>,
    fogColor: vec4<f32>,
    selectedColor: vec4<f32>,
    selected: u32,
    castedByShadows: u32,
    minHeight: f32,
    maxHeight: f32,
    heightSteps: array<vec4<f32>, 4>,
    heightScale: array<vec4<f32>, 4>,
    heightTransitionRange: f32,
    numLayers: u32,
    hasNormalMap: u32,
    hasConeMap: u32,
}

var<private> gl_FragCoord_1: vec4<f32>;
@group(0) @binding(6) 
var heightTextures: texture_2d_array<f32>;
@group(0) @binding(7) 
var heightSampler: sampler;
@group(0) @binding(2) 
var colorTextures: texture_2d_array<f32>;
@group(0) @binding(3) 
var colorSampler: sampler;
@group(0) @binding(4) 
var normalTextures: texture_2d_array<f32>;
@group(0) @binding(5) 
var normalSampler: sampler;
var<private> inToLightVectorTangentSpace0_1: vec3<f32>;
var<private> inToLightVectorTangentSpace1_1: vec3<f32>;
var<private> inToLightVectorTangentSpace2_1: vec3<f32>;
var<private> inToLightVectorTangentSpace3_1: vec3<f32>;
var<private> inClipDistance_1: f32;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> inWorldPosition_1: vec3<f32>;
var<private> inPositionTangentSpace_1: vec3<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;
@group(0) @binding(8) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(9) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inToCameraVectorTangentSpace_1: vec3<f32>;
var<private> inVisibility_1: f32;
var<private> outColor: vec4<f32>;

fn GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b(normal: ptr<function, vec3<f32>>, toLightVector: ptr<function, vec3<f32>>, toCameraVector: ptr<function, vec3<f32>>, lightColor: ptr<function, vec3<f32>>, attenuationFactor: ptr<function, f32>, shineDamper: ptr<function, f32>, reflectivity: ptr<function, f32>) -> vec3<f32> {
    var lightDirection: vec3<f32>;
    var reflectedLightDirection: vec3<f32>;
    var halfwayDir: vec3<f32>;
    var specularFactor: f32;
    var shinePower: f32;
    var dampedFactor: f32;

    let _e77 = (*toLightVector);
    lightDirection = -(_e77);
    let _e79 = lightDirection;
    let _e80 = (*normal);
    reflectedLightDirection = reflect(_e79, _e80);
    if true {
        let _e82 = (*toLightVector);
        let _e83 = (*toCameraVector);
        halfwayDir = normalize((_e82 + _e83));
        let _e86 = (*normal);
        let _e87 = halfwayDir;
        specularFactor = max(dot(_e86, _e87), 0f);
        let _e90 = (*shineDamper);
        shinePower = (_e90 * 2.4f);
    } else {
        let _e92 = reflectedLightDirection;
        let _e93 = (*toCameraVector);
        specularFactor = max(dot(_e92, _e93), 0f);
        let _e96 = (*shineDamper);
        shinePower = _e96;
    }
    let _e97 = specularFactor;
    let _e98 = shinePower;
    dampedFactor = pow(_e97, _e98);
    let _e100 = dampedFactor;
    let _e101 = (*reflectivity);
    let _e103 = (*lightColor);
    let _e105 = (*attenuationFactor);
    return ((_e103 * (_e100 * _e101)) / vec3(_e105));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e70 = (*normal_1);
    let _e71 = (*toLightVector_1);
    nDotL = dot(_e70, _e71);
    let _e73 = nDotL;
    brightness = max(_e73, 0f);
    let _e75 = brightness;
    let _e76 = (*lightColor_1);
    let _e78 = (*attenuationFactor_1);
    return ((_e76 * _e75) / vec3(_e78));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e68 = (*toLightVector_2);
    toLightDistance = length(_e68);
    let _e71 = (*attenuation)[0u];
    let _e73 = (*attenuation)[1u];
    let _e74 = toLightDistance;
    let _e78 = (*attenuation)[2u];
    let _e79 = toLightDistance;
    let _e81 = toLightDistance;
    attenuationFactor_2 = ((_e71 + (_e73 * _e74)) + ((_e78 * _e79) * _e81));
    let _e84 = attenuationFactor_2;
    return _e84;
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_130_: bool;

    shadow = 1f;
    let _e74 = (*shadowCoord)[2u];
    let _e75 = (_e74 >= 0f);
    phi_130_ = _e75;
    if _e75 {
        let _e77 = (*shadowCoord)[2u];
        phi_130_ = (_e77 <= 1f);
    }
    let _e80 = phi_130_;
    if _e80 {
        let _e81 = (*shadowCoord);
        let _e83 = (*shadowCoordOffset);
        let _e84 = (_e81.xy + _e83);
        let _e85 = (*cascadeIndex);
        let _e89 = vec3<f32>(_e84.x, _e84.y, f32(_e85));
        let _e95 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e89.x, _e89.y), i32(_e89.z), 0f);
        depth = _e95.x;
        let _e97 = (*useReverseDepth);
        if (_e97 != 0u) {
            let _e99 = depth;
            let _e101 = (*shadowCoord)[2u];
            let _e102 = (*depthBias);
            if (_e99 > (_e101 + _e102)) {
                shadow = 0.2f;
            }
        } else {
            let _e105 = depth;
            let _e107 = (*shadowCoord)[2u];
            let _e108 = (*depthBias);
            if (_e105 < (_e107 - _e108)) {
                shadow = 0.2f;
            }
        }
    }
    let _e111 = shadow;
    return _e111;
}

fn GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1: texture_2d_array<f32>, depthSampler_1: sampler, shadowCoord_1: ptr<function, vec4<f32>>, cascadeIndex_1: ptr<function, u32>, depthBias_1: ptr<function, f32>, useReverseDepth_1: ptr<function, u32>) -> f32 {
    var textureDim: vec2<f32>;
    var texelSize: vec2<f32>;
    var shadow_1: f32;
    var offset: vec2<f32>;
    var param: vec4<f32>;
    var param_1: vec2<f32>;
    var param_2: u32;
    var param_3: f32;
    var param_4: u32;
    var param_5: vec4<f32>;
    var param_6: vec2<f32>;
    var param_7: u32;
    var param_8: f32;
    var param_9: u32;
    var param_10: vec4<f32>;
    var param_11: vec2<f32>;
    var param_12: u32;
    var param_13: f32;
    var param_14: u32;
    var param_15: vec4<f32>;
    var param_16: vec2<f32>;
    var param_17: u32;
    var param_18: f32;
    var param_19: u32;

    let _e94 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e94).xy);
    let _e98 = textureDim;
    texelSize = (vec2(1f) / _e98);
    shadow_1 = 0f;
    let _e101 = gl_FragCoord_1;
    offset = (fract((_e101.xy * 0.5f)) + vec2(0.25f));
    let _e108 = offset[1u];
    if (_e108 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e111 = texelSize;
    let _e112 = offset;
    let _e115 = (*shadowCoord_1);
    param = _e115;
    param_1 = (_e111 * (_e112 + vec2<f32>(-1.5f, 0.5f)));
    let _e116 = (*cascadeIndex_1);
    param_2 = _e116;
    let _e117 = (*depthBias_1);
    param_3 = _e117;
    let _e118 = (*useReverseDepth_1);
    param_4 = _e118;
    let _e119 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e120 = texelSize;
    let _e121 = offset;
    let _e124 = (*shadowCoord_1);
    param_5 = _e124;
    param_6 = (_e120 * (_e121 + vec2<f32>(0.5f, 0.5f)));
    let _e125 = (*cascadeIndex_1);
    param_7 = _e125;
    let _e126 = (*depthBias_1);
    param_8 = _e126;
    let _e127 = (*useReverseDepth_1);
    param_9 = _e127;
    let _e128 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e130 = texelSize;
    let _e131 = offset;
    let _e134 = (*shadowCoord_1);
    param_10 = _e134;
    param_11 = (_e130 * (_e131 + vec2<f32>(-1.5f, -1.5f)));
    let _e135 = (*cascadeIndex_1);
    param_12 = _e135;
    let _e136 = (*depthBias_1);
    param_13 = _e136;
    let _e137 = (*useReverseDepth_1);
    param_14 = _e137;
    let _e138 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e140 = texelSize;
    let _e141 = offset;
    let _e144 = (*shadowCoord_1);
    param_15 = _e144;
    param_16 = (_e140 * (_e141 + vec2<f32>(0.5f, -1.5f)));
    let _e145 = (*cascadeIndex_1);
    param_17 = _e145;
    let _e146 = (*depthBias_1);
    param_18 = _e146;
    let _e147 = (*useReverseDepth_1);
    param_19 = _e147;
    let _e148 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e119 + _e128) + _e138) + _e148) * 0.25f);
    let _e151 = shadow_1;
    return _e151;
}

fn GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2: texture_2d_array<f32>, depthSampler_2: sampler, shadowCoord_2: ptr<function, vec4<f32>>, cascadeIndex_2: ptr<function, u32>, depthBias_2: ptr<function, f32>, useReverseDepth_2: ptr<function, u32>) -> f32 {
    var shadow_2: f32;
    var param_20: vec4<f32>;
    var param_21: u32;
    var param_22: f32;
    var param_23: u32;
    var param_24: vec4<f32>;
    var param_25: vec2<f32>;
    var param_26: u32;
    var param_27: f32;
    var param_28: u32;

    shadow_2 = 1f;
    if true {
        let _e80 = (*shadowCoord_2);
        param_20 = _e80;
        let _e81 = (*cascadeIndex_2);
        param_21 = _e81;
        let _e82 = (*depthBias_2);
        param_22 = _e82;
        let _e83 = (*useReverseDepth_2);
        param_23 = _e83;
        let _e84 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e84;
    } else {
        let _e85 = (*shadowCoord_2);
        param_24 = _e85;
        param_25 = vec2<f32>(0f, 0f);
        let _e86 = (*cascadeIndex_2);
        param_26 = _e86;
        let _e87 = (*depthBias_2);
        param_27 = _e87;
        let _e88 = (*useReverseDepth_2);
        param_28 = _e88;
        let _e89 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e89;
    }
    let _e90 = shadow_2;
    return _e90;
}

fn GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_3: texture_2d_array<f32>, depthSampler_3: sampler, shadows: ptr<function, Shadows>, viewPosition: ptr<function, vec3<f32>>, worldPosition: ptr<function, vec3<f32>>, depthBias_3: ptr<function, f32>) -> f32 {
    var shadow_3: f32;
    var bias: f32;
    var cascadeIndex_3: u32;
    var i: u32;
    var shadowCoord_3: vec4<f32>;
    var normalizedShadowCoord: vec4<f32>;
    var param_29: vec4<f32>;
    var param_30: u32;
    var param_31: f32;
    var param_32: u32;

    shadow_3 = 1f;
    let _e80 = (*depthBias_3);
    bias = _e80;
    let _e82 = (*shadows).enabled;
    if (_e82 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e84 = i;
            if (_e84 < 3u) {
                let _e87 = (*viewPosition)[2u];
                let _e88 = i;
                let _e93 = (*shadows).cascades[_e88].split[0u];
                if (_e87 < _e93) {
                    let _e95 = i;
                    cascadeIndex_3 = (_e95 + 1u);
                    let _e97 = bias;
                    bias = (_e97 / 1.5f);
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e99 = i;
                i = (_e99 + bitcast<u32>(1i));
            }
        }
        let _e102 = cascadeIndex_3;
        let _e106 = (*shadows).cascades[_e102].viewProjectionMatrix;
        let _e107 = (*worldPosition);
        shadowCoord_3 = (_e106 * vec4<f32>(_e107.x, _e107.y, _e107.z, 1f));
        let _e113 = shadowCoord_3;
        let _e115 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e113 / vec4(_e115));
        let _e118 = normalizedShadowCoord;
        param_29 = _e118;
        let _e119 = cascadeIndex_3;
        param_30 = _e119;
        let _e120 = bias;
        param_31 = _e120;
        let _e122 = (*shadows).useReverseDepth;
        param_32 = _e122;
        let _e123 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e123;
    }
    let _e124 = shadow_3;
    return _e124;
}

fn sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx: ptr<function, u32>, uv: ptr<function, vec2<f32>>, ddx: ptr<function, vec2<f32>>, ddy: ptr<function, vec2<f32>>) -> vec4<f32> {
    let _e68 = (*uv);
    let _e69 = (*idx);
    let _e73 = vec3<f32>(_e68.x, _e68.y, f32(_e69));
    let _e74 = (*ddx);
    let _e75 = (*ddy);
    let _e81 = textureSampleGrad(colorTextures, colorSampler, vec2<f32>(_e73.x, _e73.y), i32(_e73.z), _e74, _e75);
    return _e81;
}

fn sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx_1: ptr<function, u32>, uv_1: ptr<function, vec2<f32>>, ddx_1: ptr<function, vec2<f32>>, ddy_1: ptr<function, vec2<f32>>) -> vec3<f32> {
    var n: vec3<f32>;

    let _e69 = (*uv_1);
    let _e70 = (*idx_1);
    let _e74 = vec3<f32>(_e69.x, _e69.y, f32(_e70));
    let _e75 = (*ddx_1);
    let _e76 = (*ddy_1);
    let _e82 = textureSampleGrad(normalTextures, normalSampler, vec2<f32>(_e74.x, _e74.y), i32(_e74.z), _e75, _e76);
    n = _e82.xyz;
    let _e84 = n;
    return normalize(((normalize(_e84) * 2f) - vec3(1f)));
}

fn GetInverseHeight_u0028_f1_u003b(height: ptr<function, f32>) -> f32 {
    let _e65 = (*height);
    return (1f - _e65);
}

fn GetRayDirection_u0028_vf3_u003b_f1_u003b(viewDirection: ptr<function, vec3<f32>>, heightScale: ptr<function, f32>) -> vec3<f32> {
    var v: vec3<f32>;
    var db: f32;

    let _e68 = (*viewDirection);
    v = normalize(_e68);
    let _e71 = v[2u];
    v[2u] = abs(_e71);
    if true {
        let _e75 = v[2u];
        db = (1f - _e75);
        let _e77 = db;
        let _e78 = db;
        db = (_e78 * _e77);
        let _e80 = db;
        let _e81 = db;
        db = (_e81 * _e80);
        let _e83 = db;
        let _e84 = db;
        db = (1f - (_e83 * _e84));
        let _e87 = db;
        let _e88 = v;
        let _e90 = (_e88.xy * _e87);
        v[0u] = _e90.x;
        v[1u] = _e90.y;
    }
    let _e95 = (*heightScale);
    let _e96 = v;
    let _e98 = (_e96.xy * _e95);
    v[0u] = _e98.x;
    v[1u] = _e98.y;
    let _e103 = v;
    return _e103;
}

fn sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b(idx_2: ptr<function, u32>, heightScale_1: ptr<function, f32>, numLayers: ptr<function, u32>, uv_2: ptr<function, vec2<f32>>, ddx_2: ptr<function, vec2<f32>>, ddy_2: ptr<function, vec2<f32>>, texDir3D: ptr<function, vec3<f32>>) -> vec2<f32> {
    var rayPos: vec3<f32>;
    var rayDir: vec3<f32>;
    var param_33: vec3<f32>;
    var param_34: f32;
    var rayRatio: f32;
    var pos: vec3<f32>;
    var i_1: u32;
    var heightAndCone: vec2<f32>;
    var coneRatio: f32;
    var height_1: f32;
    var param_35: f32;
    var d: f32;
    var bsRange: vec3<f32>;
    var bsPosition: vec3<f32>;
    var i_2: u32;
    var heightAndCone_1: vec2<f32>;
    var param_36: f32;

    let _e88 = (*uv_2);
    rayPos = vec3<f32>(_e88.x, _e88.y, 0f);
    let _e92 = (*texDir3D);
    param_33 = _e92;
    let _e93 = (*heightScale_1);
    param_34 = _e93;
    let _e94 = GetRayDirection_u0028_vf3_u003b_f1_u003b((&param_33), (&param_34));
    rayDir = _e94;
    let _e96 = rayDir[2u];
    let _e97 = rayDir;
    rayDir = (_e97 / vec3(_e96));
    let _e100 = rayDir;
    rayRatio = length(_e100.xy);
    let _e103 = rayPos;
    pos = _e103;
    i_1 = 0u;
    loop {
        let _e104 = i_1;
        let _e105 = (*numLayers);
        if (_e104 < _e105) {
            let _e107 = pos;
            let _e108 = _e107.xy;
            let _e109 = (*idx_2);
            let _e113 = vec3<f32>(_e108.x, _e108.y, f32(_e109));
            let _e114 = (*ddx_2);
            let _e115 = (*ddy_2);
            let _e121 = textureSampleGrad(heightTextures, heightSampler, vec2<f32>(_e113.x, _e113.y), i32(_e113.z), _e114, _e115);
            heightAndCone = clamp(_e121.xy, vec2(0f), vec2(1f));
            let _e127 = heightAndCone[1u];
            let _e129 = heightAndCone[1u];
            coneRatio = (_e127 * _e129);
            let _e132 = heightAndCone[0u];
            param_35 = _e132;
            let _e133 = GetInverseHeight_u0028_f1_u003b((&param_35));
            let _e135 = pos[2u];
            height_1 = (_e133 - _e135);
            let _e137 = coneRatio;
            let _e138 = height_1;
            let _e140 = rayRatio;
            let _e141 = coneRatio;
            d = ((_e137 * _e138) / (_e140 + _e141));
            let _e144 = rayDir;
            let _e145 = d;
            let _e147 = pos;
            pos = (_e147 + (_e144 * _e145));
            continue;
        } else {
            break;
        }
        continuing {
            let _e149 = i_1;
            i_1 = (_e149 + bitcast<u32>(1i));
        }
    }
    let _e152 = rayDir;
    let _e155 = pos[2u];
    bsRange = ((_e152 * 0.5f) * _e155);
    let _e157 = rayPos;
    let _e158 = bsRange;
    bsPosition = (_e157 + _e158);
    i_2 = 0u;
    loop {
        let _e160 = i_2;
        if (_e160 < 6u) {
            let _e162 = pos;
            let _e163 = _e162.xy;
            let _e164 = (*idx_2);
            let _e168 = vec3<f32>(_e163.x, _e163.y, f32(_e164));
            let _e169 = (*ddx_2);
            let _e170 = (*ddy_2);
            let _e176 = textureSampleGrad(heightTextures, heightSampler, vec2<f32>(_e168.x, _e168.y), i32(_e168.z), _e169, _e170);
            heightAndCone_1 = clamp(_e176.xy, vec2(0f), vec2(1f));
            let _e181 = bsRange;
            bsRange = (_e181 * 0.5f);
            let _e184 = bsPosition[2u];
            let _e186 = heightAndCone_1[0u];
            param_36 = _e186;
            let _e187 = GetInverseHeight_u0028_f1_u003b((&param_36));
            if (_e184 < _e187) {
                let _e189 = bsRange;
                let _e190 = bsPosition;
                bsPosition = (_e190 + _e189);
            } else {
                let _e192 = bsRange;
                let _e193 = bsPosition;
                bsPosition = (_e193 - _e192);
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e195 = i_2;
            i_2 = (_e195 + bitcast<u32>(1i));
        }
    }
    let _e198 = bsPosition;
    return _e198.xy;
}

fn main_1() {
    var inToLightVectorTangentSpace_arr: array<vec3<f32>, 4>;
    var heightRange: f32;
    var normalizedHeight: f32;
    var rayDirection: vec3<f32>;
    var textureColor: vec4<f32>;
    var normal_2: vec3<f32>;
    var shineDamper_1: f32;
    var reflectivity_1: f32;
    var ddx_3: vec2<f32>;
    var ddy_3: vec2<f32>;
    var i_3: u32;
    var ratio: f32;
    var uv1_: vec2<f32>;
    var param_37: u32;
    var param_38: f32;
    var param_39: u32;
    var param_40: vec2<f32>;
    var param_41: vec2<f32>;
    var param_42: vec2<f32>;
    var param_43: vec3<f32>;
    var uv2_: vec2<f32>;
    var param_44: u32;
    var param_45: f32;
    var param_46: u32;
    var param_47: vec2<f32>;
    var param_48: vec2<f32>;
    var param_49: vec2<f32>;
    var param_50: vec3<f32>;
    var normal1_: vec3<f32>;
    var param_51: u32;
    var param_52: vec2<f32>;
    var param_53: vec2<f32>;
    var param_54: vec2<f32>;
    var normal2_: vec3<f32>;
    var param_55: u32;
    var param_56: vec2<f32>;
    var param_57: vec2<f32>;
    var param_58: vec2<f32>;
    var color1_: vec4<f32>;
    var param_59: u32;
    var param_60: vec2<f32>;
    var param_61: vec2<f32>;
    var param_62: vec2<f32>;
    var color2_: vec4<f32>;
    var param_63: u32;
    var param_64: vec2<f32>;
    var param_65: vec2<f32>;
    var param_66: vec2<f32>;
    var shineDamper1_: f32;
    var shineDamper2_: f32;
    var reflectivity1_: f32;
    var reflectivity2_: f32;
    var uv_3: vec2<f32>;
    var param_67: u32;
    var param_68: f32;
    var param_69: u32;
    var param_70: vec2<f32>;
    var param_71: vec2<f32>;
    var param_72: vec2<f32>;
    var param_73: vec3<f32>;
    var param_74: u32;
    var param_75: vec2<f32>;
    var param_76: vec2<f32>;
    var param_77: vec2<f32>;
    var param_78: u32;
    var param_79: vec2<f32>;
    var param_80: vec2<f32>;
    var param_81: vec2<f32>;
    var uv_4: vec2<f32>;
    var param_82: u32;
    var param_83: f32;
    var param_84: u32;
    var param_85: vec2<f32>;
    var param_86: vec2<f32>;
    var param_87: vec2<f32>;
    var param_88: vec3<f32>;
    var param_89: u32;
    var param_90: vec2<f32>;
    var param_91: vec2<f32>;
    var param_92: vec2<f32>;
    var param_93: u32;
    var param_94: vec2<f32>;
    var param_95: vec2<f32>;
    var param_96: vec2<f32>;
    var shadow_4: f32;
    var param_97: Shadows;
    var param_98: vec3<f32>;
    var param_99: vec3<f32>;
    var param_100: f32;
    var unitToCameraVector: vec3<f32>;
    var totalDiffuse: vec3<f32>;
    var totalSpecular: vec3<f32>;
    var i_4: u32;
    var light: Light;
    var toLightVector_3: vec3<f32>;
    var unitToLightVector: vec3<f32>;
    var attenuationFactor_3: f32;
    var param_101: vec3<f32>;
    var param_102: vec3<f32>;
    var param_103: vec3<f32>;
    var param_104: vec3<f32>;
    var param_105: vec3<f32>;
    var param_106: f32;
    var param_107: vec3<f32>;
    var param_108: vec3<f32>;
    var param_109: vec3<f32>;
    var param_110: vec3<f32>;
    var param_111: f32;
    var param_112: f32;
    var param_113: f32;
    var baseResultColor: vec4<f32>;
    var resultColor: vec4<f32>;
    var phi_783_: bool;

    let _e176 = inToLightVectorTangentSpace0_1;
    let _e177 = inToLightVectorTangentSpace1_1;
    let _e178 = inToLightVectorTangentSpace2_1;
    let _e179 = inToLightVectorTangentSpace3_1;
    inToLightVectorTangentSpace_arr = array<vec3<f32>, 4>(_e176, _e177, _e178, _e179);
    let _e181 = inClipDistance_1;
    if (_e181 < 0f) {
        discard;
    }
    let _e184 = uboFS.maxHeight;
    let _e187 = uboFS.minHeight;
    heightRange = (abs(_e184) + abs(_e187));
    let _e191 = inWorldPosition_1[1u];
    let _e193 = uboFS.minHeight;
    let _e196 = heightRange;
    normalizedHeight = ((_e191 + abs(_e193)) / _e196);
    let _e198 = inPositionTangentSpace_1;
    rayDirection = normalize(_e198);
    textureColor = vec4<f32>(1f, 1f, 1f, 1f);
    normal_2 = vec3<f32>(0f, 1f, 0f);
    shineDamper_1 = 1f;
    reflectivity_1 = 1f;
    let _e200 = inTextureCoord_1;
    let _e201 = dpdx(_e200);
    ddx_3 = _e201;
    let _e202 = inTextureCoord_1;
    let _e203 = dpdy(_e202);
    ddy_3 = _e203;
    i_3 = 0u;
    loop {
        let _e204 = i_3;
        if (_e204 < 4u) {
            let _e206 = i_3;
            if (_e206 < 3u) {
                let _e208 = normalizedHeight;
                let _e209 = i_3;
                let _e213 = uboFS.heightSteps[_e209][0u];
                let _e215 = uboFS.heightTransitionRange;
                let _e217 = (_e208 > (_e213 - _e215));
                phi_783_ = _e217;
                if _e217 {
                    let _e218 = normalizedHeight;
                    let _e219 = i_3;
                    let _e223 = uboFS.heightSteps[_e219][0u];
                    let _e225 = uboFS.heightTransitionRange;
                    phi_783_ = (_e218 < (_e223 + _e225));
                }
                let _e229 = phi_783_;
                if _e229 {
                    let _e230 = normalizedHeight;
                    let _e231 = i_3;
                    let _e235 = uboFS.heightSteps[_e231][0u];
                    let _e238 = uboFS.heightTransitionRange;
                    let _e241 = uboFS.heightTransitionRange;
                    ratio = (((_e230 - _e235) + _e238) / (2f * _e241));
                    let _e245 = uboFS.hasConeMap;
                    if (_e245 != 0u) {
                        let _e247 = i_3;
                        let _e248 = i_3;
                        param_37 = _e248;
                        let _e252 = uboFS.heightScale[_e247][0u];
                        param_38 = _e252;
                        let _e254 = uboFS.numLayers;
                        param_39 = _e254;
                        let _e255 = inTextureCoord_1;
                        param_40 = _e255;
                        let _e256 = ddx_3;
                        param_41 = _e256;
                        let _e257 = ddy_3;
                        param_42 = _e257;
                        let _e258 = rayDirection;
                        param_43 = _e258;
                        let _e259 = sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b((&param_37), (&param_38), (&param_39), (&param_40), (&param_41), (&param_42), (&param_43));
                        uv1_ = _e259;
                        let _e260 = i_3;
                        let _e262 = i_3;
                        param_44 = (_e260 + 1u);
                        let _e267 = uboFS.heightScale[(_e262 + 1u)][0u];
                        param_45 = _e267;
                        let _e269 = uboFS.numLayers;
                        param_46 = _e269;
                        let _e270 = inTextureCoord_1;
                        param_47 = _e270;
                        let _e271 = ddx_3;
                        param_48 = _e271;
                        let _e272 = ddy_3;
                        param_49 = _e272;
                        let _e273 = rayDirection;
                        param_50 = _e273;
                        let _e274 = sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b((&param_44), (&param_45), (&param_46), (&param_47), (&param_48), (&param_49), (&param_50));
                        uv2_ = _e274;
                    } else {
                        let _e275 = inTextureCoord_1;
                        uv1_ = _e275;
                        let _e276 = inTextureCoord_1;
                        uv2_ = _e276;
                    }
                    let _e278 = uboFS.hasNormalMap;
                    if (_e278 != 0u) {
                        let _e280 = i_3;
                        param_51 = _e280;
                        let _e281 = uv1_;
                        param_52 = _e281;
                        let _e282 = ddx_3;
                        param_53 = _e282;
                        let _e283 = ddy_3;
                        param_54 = _e283;
                        let _e284 = sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_51), (&param_52), (&param_53), (&param_54));
                        normal1_ = _e284;
                        let _e285 = i_3;
                        param_55 = (_e285 + 1u);
                        let _e287 = uv2_;
                        param_56 = _e287;
                        let _e288 = ddx_3;
                        param_57 = _e288;
                        let _e289 = ddy_3;
                        param_58 = _e289;
                        let _e290 = sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_55), (&param_56), (&param_57), (&param_58));
                        normal2_ = _e290;
                    } else {
                        let _e291 = inNormal_1;
                        normal1_ = _e291;
                        let _e292 = inNormal_1;
                        normal2_ = _e292;
                    }
                    let _e293 = normal1_;
                    let _e294 = normal2_;
                    let _e295 = ratio;
                    normal_2 = mix(_e293, _e294, vec3(_e295));
                    let _e298 = i_3;
                    param_59 = _e298;
                    let _e299 = uv1_;
                    param_60 = _e299;
                    let _e300 = ddx_3;
                    param_61 = _e300;
                    let _e301 = ddy_3;
                    param_62 = _e301;
                    let _e302 = sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_59), (&param_60), (&param_61), (&param_62));
                    color1_ = _e302;
                    let _e303 = i_3;
                    param_63 = (_e303 + 1u);
                    let _e305 = uv2_;
                    param_64 = _e305;
                    let _e306 = ddx_3;
                    param_65 = _e306;
                    let _e307 = ddy_3;
                    param_66 = _e307;
                    let _e308 = sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_63), (&param_64), (&param_65), (&param_66));
                    color2_ = _e308;
                    let _e309 = color1_;
                    let _e310 = color2_;
                    let _e311 = ratio;
                    textureColor = mix(_e309, _e310, vec4(_e311));
                    let _e314 = i_3;
                    let _e318 = uboFS.material[_e314].shineDamper;
                    shineDamper1_ = _e318;
                    let _e319 = i_3;
                    let _e324 = uboFS.material[(_e319 + 1u)].shineDamper;
                    shineDamper2_ = _e324;
                    let _e325 = shineDamper1_;
                    let _e326 = shineDamper2_;
                    let _e327 = ratio;
                    shineDamper_1 = mix(_e325, _e326, _e327);
                    let _e329 = i_3;
                    let _e333 = uboFS.material[_e329].reflectivity;
                    reflectivity1_ = _e333;
                    let _e334 = i_3;
                    let _e339 = uboFS.material[(_e334 + 1u)].reflectivity;
                    reflectivity2_ = _e339;
                    let _e340 = reflectivity1_;
                    let _e341 = reflectivity2_;
                    let _e342 = ratio;
                    reflectivity_1 = mix(_e340, _e341, _e342);
                    break;
                } else {
                    let _e344 = normalizedHeight;
                    let _e345 = i_3;
                    let _e349 = uboFS.heightSteps[_e345][0u];
                    let _e351 = uboFS.heightTransitionRange;
                    if (_e344 < (_e349 - _e351)) {
                        let _e354 = i_3;
                        let _e355 = i_3;
                        param_67 = _e355;
                        let _e359 = uboFS.heightScale[_e354][0u];
                        param_68 = _e359;
                        let _e361 = uboFS.numLayers;
                        param_69 = _e361;
                        let _e362 = inTextureCoord_1;
                        param_70 = _e362;
                        let _e363 = ddx_3;
                        param_71 = _e363;
                        let _e364 = ddy_3;
                        param_72 = _e364;
                        let _e365 = rayDirection;
                        param_73 = _e365;
                        let _e366 = sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b((&param_67), (&param_68), (&param_69), (&param_70), (&param_71), (&param_72), (&param_73));
                        uv_3 = _e366;
                        let _e367 = i_3;
                        param_74 = _e367;
                        let _e368 = uv_3;
                        param_75 = _e368;
                        let _e369 = ddx_3;
                        param_76 = _e369;
                        let _e370 = ddy_3;
                        param_77 = _e370;
                        let _e371 = sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_74), (&param_75), (&param_76), (&param_77));
                        normal_2 = _e371;
                        let _e372 = i_3;
                        param_78 = _e372;
                        let _e373 = uv_3;
                        param_79 = _e373;
                        let _e374 = ddx_3;
                        param_80 = _e374;
                        let _e375 = ddy_3;
                        param_81 = _e375;
                        let _e376 = sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_78), (&param_79), (&param_80), (&param_81));
                        textureColor = _e376;
                        let _e377 = i_3;
                        let _e381 = uboFS.material[_e377].shineDamper;
                        shineDamper_1 = _e381;
                        let _e382 = i_3;
                        let _e386 = uboFS.material[_e382].reflectivity;
                        reflectivity_1 = _e386;
                        break;
                    }
                }
            } else {
                let _e387 = i_3;
                let _e388 = i_3;
                param_82 = _e388;
                let _e392 = uboFS.heightScale[_e387][0u];
                param_83 = _e392;
                let _e394 = uboFS.numLayers;
                param_84 = _e394;
                let _e395 = inTextureCoord_1;
                param_85 = _e395;
                let _e396 = ddx_3;
                param_86 = _e396;
                let _e397 = ddy_3;
                param_87 = _e397;
                let _e398 = rayDirection;
                param_88 = _e398;
                let _e399 = sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b((&param_82), (&param_83), (&param_84), (&param_85), (&param_86), (&param_87), (&param_88));
                uv_4 = _e399;
                let _e400 = i_3;
                param_89 = _e400;
                let _e401 = uv_4;
                param_90 = _e401;
                let _e402 = ddx_3;
                param_91 = _e402;
                let _e403 = ddy_3;
                param_92 = _e403;
                let _e404 = sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_89), (&param_90), (&param_91), (&param_92));
                normal_2 = _e404;
                let _e405 = i_3;
                param_93 = _e405;
                let _e406 = uv_4;
                param_94 = _e406;
                let _e407 = ddx_3;
                param_95 = _e407;
                let _e408 = ddy_3;
                param_96 = _e408;
                let _e409 = sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_93), (&param_94), (&param_95), (&param_96));
                textureColor = _e409;
                let _e410 = i_3;
                let _e414 = uboFS.material[_e410].shineDamper;
                shineDamper_1 = _e414;
                let _e415 = i_3;
                let _e419 = uboFS.material[_e415].reflectivity;
                reflectivity_1 = _e419;
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e420 = i_3;
            i_3 = (_e420 + bitcast<u32>(1i));
        }
    }
    shadow_4 = 1f;
    let _e424 = uboFS.castedByShadows;
    if (_e424 != 0u) {
        let _e427 = uboFS.shadows;
        param_97.cascades[0i].viewProjectionMatrix = _e427.cascades[0].viewProjectionMatrix;
        param_97.cascades[0i].split = _e427.cascades[0].split;
        param_97.cascades[1i].viewProjectionMatrix = _e427.cascades[1].viewProjectionMatrix;
        param_97.cascades[1i].split = _e427.cascades[1].split;
        param_97.cascades[2i].viewProjectionMatrix = _e427.cascades[2].viewProjectionMatrix;
        param_97.cascades[2i].split = _e427.cascades[2].split;
        param_97.cascades[3i].viewProjectionMatrix = _e427.cascades[3].viewProjectionMatrix;
        param_97.cascades[3i].split = _e427.cascades[3].split;
        param_97.enabled = _e427.enabled;
        param_97.useReverseDepth = _e427.useReverseDepth;
        let _e458 = inViewPosition_1;
        param_98 = _e458;
        let _e459 = inWorldPosition_1;
        param_99 = _e459;
        param_100 = 0.02f;
        let _e460 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_97), (&param_98), (&param_99), (&param_100));
        shadow_4 = _e460;
    }
    let _e461 = inToCameraVectorTangentSpace_1;
    let _e462 = inPositionTangentSpace_1;
    unitToCameraVector = normalize((_e461 - _e462));
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_4 = 0u;
    loop {
        let _e465 = i_4;
        let _e468 = uboFS.lightning.realCountOfLights;
        if (_e465 < _e468) {
            let _e470 = i_4;
            let _e474 = uboFS.lightning.lights[_e470];
            light.position = _e474.position;
            light.color = _e474.color;
            light.attenuation = _e474.attenuation;
            let _e481 = i_4;
            let _e483 = inToLightVectorTangentSpace_arr[_e481];
            let _e484 = inPositionTangentSpace_1;
            toLightVector_3 = (_e483 - _e484);
            let _e486 = toLightVector_3;
            unitToLightVector = normalize(_e486);
            let _e489 = light.attenuation;
            param_101 = _e489.xyz;
            let _e491 = toLightVector_3;
            param_102 = _e491;
            let _e492 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_101), (&param_102));
            attenuationFactor_3 = _e492;
            let _e493 = normal_2;
            param_103 = _e493;
            let _e494 = unitToLightVector;
            param_104 = _e494;
            let _e496 = light.color;
            param_105 = _e496.xyz;
            let _e498 = attenuationFactor_3;
            param_106 = _e498;
            let _e499 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_103), (&param_104), (&param_105), (&param_106));
            let _e500 = totalDiffuse;
            totalDiffuse = (_e500 + _e499);
            let _e502 = normal_2;
            param_107 = _e502;
            let _e503 = unitToLightVector;
            param_108 = _e503;
            let _e504 = unitToCameraVector;
            param_109 = _e504;
            let _e506 = light.color;
            param_110 = _e506.xyz;
            let _e508 = attenuationFactor_3;
            param_111 = _e508;
            let _e509 = shineDamper_1;
            param_112 = _e509;
            let _e510 = reflectivity_1;
            param_113 = _e510;
            let _e511 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_107), (&param_108), (&param_109), (&param_110), (&param_111), (&param_112), (&param_113));
            let _e512 = totalSpecular;
            totalSpecular = (_e512 + _e511);
            continue;
        } else {
            break;
        }
        continuing {
            let _e514 = i_4;
            i_4 = (_e514 + bitcast<u32>(1i));
        }
    }
    let _e517 = totalDiffuse;
    let _e518 = shadow_4;
    let _e522 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e517 * _e518) + vec3(_e522));
    let _e525 = totalSpecular;
    let _e526 = shadow_4;
    totalSpecular = (_e525 * _e526);
    let _e528 = totalDiffuse;
    let _e533 = textureColor;
    let _e535 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e528.x, _e528.y, _e528.z, 1f) * _e533) + vec4<f32>(_e535.x, _e535.y, _e535.z, 0f));
    let _e542 = uboFS.fogColor;
    let _e543 = _e542.xyz;
    let _e548 = baseResultColor;
    let _e549 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e543.x, _e543.y, _e543.z, 1f), _e548, vec4(_e549));
    let _e553 = uboFS.selected;
    if (_e553 != 0u) {
        let _e555 = resultColor;
        let _e557 = uboFS.selectedColor;
        resultColor = mix(_e555, _e557, vec4(0.5f));
    }
    let _e560 = resultColor;
    outColor = _e560;
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(7) inToLightVectorTangentSpace0_: vec3<f32>, @location(8) inToLightVectorTangentSpace1_: vec3<f32>, @location(9) inToLightVectorTangentSpace2_: vec3<f32>, @location(10) inToLightVectorTangentSpace3_: vec3<f32>, @location(11) inClipDistance: f32, @location(2) inWorldPosition: vec3<f32>, @location(6) inPositionTangentSpace: vec3<f32>, @location(0) inTextureCoord: vec2<f32>, @location(1) inNormal: vec3<f32>, @location(3) inViewPosition: vec3<f32>, @location(5) inToCameraVectorTangentSpace: vec3<f32>, @location(4) inVisibility: f32) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inToLightVectorTangentSpace0_1 = inToLightVectorTangentSpace0_;
    inToLightVectorTangentSpace1_1 = inToLightVectorTangentSpace1_;
    inToLightVectorTangentSpace2_1 = inToLightVectorTangentSpace2_;
    inToLightVectorTangentSpace3_1 = inToLightVectorTangentSpace3_;
    inClipDistance_1 = inClipDistance;
    inWorldPosition_1 = inWorldPosition;
    inPositionTangentSpace_1 = inPositionTangentSpace;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    inViewPosition_1 = inViewPosition;
    inToCameraVectorTangentSpace_1 = inToCameraVectorTangentSpace;
    inVisibility_1 = inVisibility;
    main_1();
    let _e27 = outColor;
    return _e27;
}
