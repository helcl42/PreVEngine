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
    material: Material,
    fogColor: vec4<f32>,
    selectedColor: vec4<f32>,
    selected: u32,
    castedByShadows: u32,
    heightScale: f32,
    numLayers: u32,
    hasNormalMap: u32,
    hasConeMap: u32,
}

var<private> gl_FragCoord_1: vec4<f32>;
var<private> inToLightVectorTangentSpace0_1: vec3<f32>;
var<private> inToLightVectorTangentSpace1_1: vec3<f32>;
var<private> inToLightVectorTangentSpace2_1: vec3<f32>;
var<private> inToLightVectorTangentSpace3_1: vec3<f32>;
var<private> inClipDistance_1: f32;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inPositionTangentSpace_1: vec3<f32>;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
@group(0) @binding(6) 
var heightTexture: texture_2d<f32>;
@group(0) @binding(7) 
var heightSampler: sampler;
@group(0) @binding(8) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(9) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inWorldPosition_1: vec3<f32>;
@group(0) @binding(4) 
var normalTexture: texture_2d<f32>;
@group(0) @binding(5) 
var normalSampler: sampler;
var<private> inNormal_1: vec3<f32>;
@group(0) @binding(2) 
var colorTexture: texture_2d<f32>;
@group(0) @binding(3) 
var colorSampler: sampler;
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

    let _e71 = (*toLightVector);
    lightDirection = -(_e71);
    let _e73 = lightDirection;
    let _e74 = (*normal);
    reflectedLightDirection = reflect(_e73, _e74);
    if true {
        let _e76 = (*toLightVector);
        let _e77 = (*toCameraVector);
        halfwayDir = normalize((_e76 + _e77));
        let _e80 = (*normal);
        let _e81 = halfwayDir;
        specularFactor = max(dot(_e80, _e81), 0f);
        let _e84 = (*shineDamper);
        shinePower = (_e84 * 2.4f);
    } else {
        let _e86 = reflectedLightDirection;
        let _e87 = (*toCameraVector);
        specularFactor = max(dot(_e86, _e87), 0f);
        let _e90 = (*shineDamper);
        shinePower = _e90;
    }
    let _e91 = specularFactor;
    let _e92 = shinePower;
    dampedFactor = pow(_e91, _e92);
    let _e94 = dampedFactor;
    let _e95 = (*reflectivity);
    let _e97 = (*lightColor);
    let _e99 = (*attenuationFactor);
    return ((_e97 * (_e94 * _e95)) / vec3(_e99));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e64 = (*normal_1);
    let _e65 = (*toLightVector_1);
    nDotL = dot(_e64, _e65);
    let _e67 = nDotL;
    brightness = max(_e67, 0f);
    let _e69 = brightness;
    let _e70 = (*lightColor_1);
    let _e72 = (*attenuationFactor_1);
    return ((_e70 * _e69) / vec3(_e72));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e62 = (*toLightVector_2);
    toLightDistance = length(_e62);
    let _e65 = (*attenuation)[0u];
    let _e67 = (*attenuation)[1u];
    let _e68 = toLightDistance;
    let _e72 = (*attenuation)[2u];
    let _e73 = toLightDistance;
    let _e75 = toLightDistance;
    attenuationFactor_2 = ((_e65 + (_e67 * _e68)) + ((_e72 * _e73) * _e75));
    let _e78 = attenuationFactor_2;
    return _e78;
}

fn NormalMapping_u0028_t21_u003b_p1_u003b_vf2_u003b(normalMapTexture: texture_2d<f32>, normalMapSampler: sampler, uv: ptr<function, vec2<f32>>) -> vec3<f32> {
    let _e61 = (*uv);
    let _e62 = textureSample(normalMapTexture, normalMapSampler, _e61);
    return normalize(((normalize(_e62.xyz) * 2f) - vec3(1f)));
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_125_: bool;

    shadow = 1f;
    let _e68 = (*shadowCoord)[2u];
    let _e69 = (_e68 >= 0f);
    phi_125_ = _e69;
    if _e69 {
        let _e71 = (*shadowCoord)[2u];
        phi_125_ = (_e71 <= 1f);
    }
    let _e74 = phi_125_;
    if _e74 {
        let _e75 = (*shadowCoord);
        let _e77 = (*shadowCoordOffset);
        let _e78 = (_e75.xy + _e77);
        let _e79 = (*cascadeIndex);
        let _e83 = vec3<f32>(_e78.x, _e78.y, f32(_e79));
        let _e89 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e83.x, _e83.y), i32(_e83.z), 0f);
        depth = _e89.x;
        let _e91 = (*useReverseDepth);
        if (_e91 != 0u) {
            let _e93 = depth;
            let _e95 = (*shadowCoord)[2u];
            let _e96 = (*depthBias);
            if (_e93 > (_e95 + _e96)) {
                shadow = 0.2f;
            }
        } else {
            let _e99 = depth;
            let _e101 = (*shadowCoord)[2u];
            let _e102 = (*depthBias);
            if (_e99 < (_e101 - _e102)) {
                shadow = 0.2f;
            }
        }
    }
    let _e105 = shadow;
    return _e105;
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

    let _e88 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e88).xy);
    let _e92 = textureDim;
    texelSize = (vec2(1f) / _e92);
    shadow_1 = 0f;
    let _e95 = gl_FragCoord_1;
    offset = (fract((_e95.xy * 0.5f)) + vec2(0.25f));
    let _e102 = offset[1u];
    if (_e102 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e105 = texelSize;
    let _e106 = offset;
    let _e109 = (*shadowCoord_1);
    param = _e109;
    param_1 = (_e105 * (_e106 + vec2<f32>(-1.5f, 0.5f)));
    let _e110 = (*cascadeIndex_1);
    param_2 = _e110;
    let _e111 = (*depthBias_1);
    param_3 = _e111;
    let _e112 = (*useReverseDepth_1);
    param_4 = _e112;
    let _e113 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e114 = texelSize;
    let _e115 = offset;
    let _e118 = (*shadowCoord_1);
    param_5 = _e118;
    param_6 = (_e114 * (_e115 + vec2<f32>(0.5f, 0.5f)));
    let _e119 = (*cascadeIndex_1);
    param_7 = _e119;
    let _e120 = (*depthBias_1);
    param_8 = _e120;
    let _e121 = (*useReverseDepth_1);
    param_9 = _e121;
    let _e122 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e124 = texelSize;
    let _e125 = offset;
    let _e128 = (*shadowCoord_1);
    param_10 = _e128;
    param_11 = (_e124 * (_e125 + vec2<f32>(-1.5f, -1.5f)));
    let _e129 = (*cascadeIndex_1);
    param_12 = _e129;
    let _e130 = (*depthBias_1);
    param_13 = _e130;
    let _e131 = (*useReverseDepth_1);
    param_14 = _e131;
    let _e132 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e134 = texelSize;
    let _e135 = offset;
    let _e138 = (*shadowCoord_1);
    param_15 = _e138;
    param_16 = (_e134 * (_e135 + vec2<f32>(0.5f, -1.5f)));
    let _e139 = (*cascadeIndex_1);
    param_17 = _e139;
    let _e140 = (*depthBias_1);
    param_18 = _e140;
    let _e141 = (*useReverseDepth_1);
    param_19 = _e141;
    let _e142 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e113 + _e122) + _e132) + _e142) * 0.25f);
    let _e145 = shadow_1;
    return _e145;
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
        let _e74 = (*shadowCoord_2);
        param_20 = _e74;
        let _e75 = (*cascadeIndex_2);
        param_21 = _e75;
        let _e76 = (*depthBias_2);
        param_22 = _e76;
        let _e77 = (*useReverseDepth_2);
        param_23 = _e77;
        let _e78 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e78;
    } else {
        let _e79 = (*shadowCoord_2);
        param_24 = _e79;
        param_25 = vec2<f32>(0f, 0f);
        let _e80 = (*cascadeIndex_2);
        param_26 = _e80;
        let _e81 = (*depthBias_2);
        param_27 = _e81;
        let _e82 = (*useReverseDepth_2);
        param_28 = _e82;
        let _e83 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e83;
    }
    let _e84 = shadow_2;
    return _e84;
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
    let _e74 = (*depthBias_3);
    bias = _e74;
    let _e76 = (*shadows).enabled;
    if (_e76 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e78 = i;
            if (_e78 < 3u) {
                let _e81 = (*shadows).useReverseDepth;
                if (_e81 != 0u) {
                    let _e84 = (*viewPosition)[2u];
                    let _e85 = i;
                    let _e90 = (*shadows).cascades[_e85].split[0u];
                    if (_e84 > _e90) {
                        let _e92 = i;
                        cascadeIndex_3 = (_e92 + 1u);
                        let _e94 = bias;
                        bias = (_e94 / 1.5f);
                    }
                } else {
                    let _e97 = (*viewPosition)[2u];
                    let _e98 = i;
                    let _e103 = (*shadows).cascades[_e98].split[0u];
                    if (_e97 < _e103) {
                        let _e105 = i;
                        cascadeIndex_3 = (_e105 + 1u);
                        let _e107 = bias;
                        bias = (_e107 / 1.5f);
                    }
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e109 = i;
                i = (_e109 + bitcast<u32>(1i));
            }
        }
        let _e112 = cascadeIndex_3;
        let _e116 = (*shadows).cascades[_e112].viewProjectionMatrix;
        let _e117 = (*worldPosition);
        shadowCoord_3 = (_e116 * vec4<f32>(_e117.x, _e117.y, _e117.z, 1f));
        let _e123 = shadowCoord_3;
        let _e125 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e123 / vec4(_e125));
        let _e128 = normalizedShadowCoord;
        param_29 = _e128;
        let _e129 = cascadeIndex_3;
        param_30 = _e129;
        let _e130 = bias;
        param_31 = _e130;
        let _e132 = (*shadows).useReverseDepth;
        param_32 = _e132;
        let _e133 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e133;
    }
    let _e134 = shadow_3;
    return _e134;
}

fn GetInverseHeight_u0028_f1_u003b(height: ptr<function, f32>) -> f32 {
    let _e59 = (*height);
    return (1f - _e59);
}

fn GetRayDirection_u0028_vf3_u003b_f1_u003b(viewDirection: ptr<function, vec3<f32>>, heightScale: ptr<function, f32>) -> vec3<f32> {
    var v: vec3<f32>;
    var db: f32;

    let _e62 = (*viewDirection);
    v = normalize(_e62);
    let _e65 = v[2u];
    v[2u] = abs(_e65);
    if true {
        let _e69 = v[2u];
        db = (1f - _e69);
        let _e71 = db;
        let _e72 = db;
        db = (_e72 * _e71);
        let _e74 = db;
        let _e75 = db;
        db = (_e75 * _e74);
        let _e77 = db;
        let _e78 = db;
        db = (1f - (_e77 * _e78));
        let _e81 = db;
        let _e82 = v;
        let _e84 = (_e82.xy * _e81);
        v[0u] = _e84.x;
        v[1u] = _e84.y;
    }
    let _e89 = (*heightScale);
    let _e90 = v;
    let _e92 = (_e90.xy * _e89);
    v[0u] = _e92.x;
    v[1u] = _e92.y;
    let _e97 = v;
    return _e97;
}

fn RelaxedConeStepMapping_u0028_t21_u003b_p1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b(coneMapTexture: texture_2d<f32>, coneMapSampler: sampler, heightScale_1: ptr<function, f32>, numLayers: ptr<function, u32>, uv_1: ptr<function, vec2<f32>>, ddx: ptr<function, vec2<f32>>, ddy: ptr<function, vec2<f32>>, texDir3D: ptr<function, vec3<f32>>) -> vec2<f32> {
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

    let _e83 = (*uv_1);
    rayPos = vec3<f32>(_e83.x, _e83.y, 0f);
    let _e87 = (*texDir3D);
    param_33 = _e87;
    let _e88 = (*heightScale_1);
    param_34 = _e88;
    let _e89 = GetRayDirection_u0028_vf3_u003b_f1_u003b((&param_33), (&param_34));
    rayDir = _e89;
    let _e91 = rayDir[2u];
    let _e92 = rayDir;
    rayDir = (_e92 / vec3(_e91));
    let _e95 = rayDir;
    rayRatio = length(_e95.xy);
    let _e98 = rayPos;
    pos = _e98;
    i_1 = 0u;
    loop {
        let _e99 = i_1;
        let _e100 = (*numLayers);
        if (_e99 < _e100) {
            let _e102 = pos;
            let _e104 = (*ddx);
            let _e105 = (*ddy);
            let _e106 = textureSampleGrad(coneMapTexture, coneMapSampler, _e102.xy, _e104, _e105);
            heightAndCone = clamp(_e106.xy, vec2(0f), vec2(1f));
            let _e112 = heightAndCone[1u];
            let _e114 = heightAndCone[1u];
            coneRatio = (_e112 * _e114);
            let _e117 = heightAndCone[0u];
            param_35 = _e117;
            let _e118 = GetInverseHeight_u0028_f1_u003b((&param_35));
            let _e120 = pos[2u];
            height_1 = (_e118 - _e120);
            let _e122 = coneRatio;
            let _e123 = height_1;
            let _e125 = rayRatio;
            let _e126 = coneRatio;
            d = ((_e122 * _e123) / (_e125 + _e126));
            let _e129 = rayDir;
            let _e130 = d;
            let _e132 = pos;
            pos = (_e132 + (_e129 * _e130));
            continue;
        } else {
            break;
        }
        continuing {
            let _e134 = i_1;
            i_1 = (_e134 + bitcast<u32>(1i));
        }
    }
    let _e137 = rayDir;
    let _e140 = pos[2u];
    bsRange = ((_e137 * 0.5f) * _e140);
    let _e142 = rayPos;
    let _e143 = bsRange;
    bsPosition = (_e142 + _e143);
    i_2 = 0u;
    loop {
        let _e145 = i_2;
        if (_e145 < 6u) {
            let _e147 = pos;
            let _e149 = (*ddx);
            let _e150 = (*ddy);
            let _e151 = textureSampleGrad(coneMapTexture, coneMapSampler, _e147.xy, _e149, _e150);
            heightAndCone_1 = clamp(_e151.xy, vec2(0f), vec2(1f));
            let _e156 = bsRange;
            bsRange = (_e156 * 0.5f);
            let _e159 = bsPosition[2u];
            let _e161 = heightAndCone_1[0u];
            param_36 = _e161;
            let _e162 = GetInverseHeight_u0028_f1_u003b((&param_36));
            if (_e159 < _e162) {
                let _e164 = bsRange;
                let _e165 = bsPosition;
                bsPosition = (_e165 + _e164);
            } else {
                let _e167 = bsRange;
                let _e168 = bsPosition;
                bsPosition = (_e168 - _e167);
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e170 = i_2;
            i_2 = (_e170 + bitcast<u32>(1i));
        }
    }
    let _e173 = bsPosition;
    return _e173.xy;
}

fn main_1() {
    var inToLightVectorTangentSpace_arr: array<vec3<f32>, 4>;
    var ddx_1: vec2<f32>;
    var ddy_1: vec2<f32>;
    var rayDirection: vec3<f32>;
    var uv_2: vec2<f32>;
    var local: vec2<f32>;
    var param_37: f32;
    var param_38: u32;
    var param_39: vec2<f32>;
    var param_40: vec2<f32>;
    var param_41: vec2<f32>;
    var param_42: vec3<f32>;
    var shadow_4: f32;
    var param_43: Shadows;
    var param_44: vec3<f32>;
    var param_45: vec3<f32>;
    var param_46: f32;
    var normal_2: vec3<f32>;
    var local_1: vec3<f32>;
    var param_47: vec2<f32>;
    var textureColor: vec4<f32>;
    var unitToCameraVector: vec3<f32>;
    var totalDiffuse: vec3<f32>;
    var totalSpecular: vec3<f32>;
    var i_3: u32;
    var light: Light;
    var toLightVector_3: vec3<f32>;
    var unitToLightVector: vec3<f32>;
    var attenuationFactor_3: f32;
    var param_48: vec3<f32>;
    var param_49: vec3<f32>;
    var param_50: vec3<f32>;
    var param_51: vec3<f32>;
    var param_52: vec3<f32>;
    var param_53: f32;
    var param_54: vec3<f32>;
    var param_55: vec3<f32>;
    var param_56: vec3<f32>;
    var param_57: vec3<f32>;
    var param_58: f32;
    var param_59: f32;
    var param_60: f32;
    var baseResultColor: vec4<f32>;
    var resultColor: vec4<f32>;

    let _e102 = inToLightVectorTangentSpace0_1;
    let _e103 = inToLightVectorTangentSpace1_1;
    let _e104 = inToLightVectorTangentSpace2_1;
    let _e105 = inToLightVectorTangentSpace3_1;
    inToLightVectorTangentSpace_arr = array<vec3<f32>, 4>(_e102, _e103, _e104, _e105);
    let _e107 = inClipDistance_1;
    if (_e107 < 0f) {
        discard;
    }
    let _e109 = inTextureCoord_1;
    let _e110 = dpdx(_e109);
    ddx_1 = _e110;
    let _e111 = inTextureCoord_1;
    let _e112 = dpdy(_e111);
    ddy_1 = _e112;
    let _e113 = inPositionTangentSpace_1;
    rayDirection = normalize(_e113);
    let _e116 = uboFS.hasConeMap;
    if (_e116 != 0u) {
        let _e119 = uboFS.heightScale;
        param_37 = _e119;
        let _e121 = uboFS.numLayers;
        param_38 = _e121;
        let _e122 = inTextureCoord_1;
        param_39 = _e122;
        let _e123 = ddx_1;
        param_40 = _e123;
        let _e124 = ddy_1;
        param_41 = _e124;
        let _e125 = rayDirection;
        param_42 = _e125;
        let _e126 = RelaxedConeStepMapping_u0028_t21_u003b_p1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b(heightTexture, heightSampler, (&param_37), (&param_38), (&param_39), (&param_40), (&param_41), (&param_42));
        local = _e126;
    } else {
        let _e127 = inTextureCoord_1;
        local = _e127;
    }
    let _e128 = local;
    uv_2 = _e128;
    shadow_4 = 1f;
    let _e130 = uboFS.castedByShadows;
    if (_e130 != 0u) {
        let _e133 = uboFS.shadows;
        param_43.cascades[0i].viewProjectionMatrix = _e133.cascades[0].viewProjectionMatrix;
        param_43.cascades[0i].split = _e133.cascades[0].split;
        param_43.cascades[1i].viewProjectionMatrix = _e133.cascades[1].viewProjectionMatrix;
        param_43.cascades[1i].split = _e133.cascades[1].split;
        param_43.cascades[2i].viewProjectionMatrix = _e133.cascades[2].viewProjectionMatrix;
        param_43.cascades[2i].split = _e133.cascades[2].split;
        param_43.cascades[3i].viewProjectionMatrix = _e133.cascades[3].viewProjectionMatrix;
        param_43.cascades[3i].split = _e133.cascades[3].split;
        param_43.enabled = _e133.enabled;
        param_43.useReverseDepth = _e133.useReverseDepth;
        let _e164 = inViewPosition_1;
        param_44 = _e164;
        let _e165 = inWorldPosition_1;
        param_45 = _e165;
        param_46 = 0.005f;
        let _e166 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_43), (&param_44), (&param_45), (&param_46));
        shadow_4 = _e166;
    }
    let _e168 = uboFS.hasNormalMap;
    if (_e168 != 0u) {
        let _e170 = uv_2;
        param_47 = _e170;
        let _e171 = NormalMapping_u0028_t21_u003b_p1_u003b_vf2_u003b(normalTexture, normalSampler, (&param_47));
        local_1 = _e171;
    } else {
        let _e172 = inNormal_1;
        local_1 = _e172;
    }
    let _e173 = local_1;
    normal_2 = _e173;
    let _e174 = uv_2;
    let _e175 = textureSample(colorTexture, colorSampler, _e174);
    textureColor = _e175;
    let _e176 = inToCameraVectorTangentSpace_1;
    let _e177 = inPositionTangentSpace_1;
    unitToCameraVector = normalize((_e176 - _e177));
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_3 = 0u;
    loop {
        let _e180 = i_3;
        let _e183 = uboFS.lightning.realCountOfLights;
        if (_e180 < _e183) {
            let _e185 = i_3;
            let _e189 = uboFS.lightning.lights[_e185];
            light.position = _e189.position;
            light.color = _e189.color;
            light.attenuation = _e189.attenuation;
            let _e196 = i_3;
            let _e198 = inToLightVectorTangentSpace_arr[_e196];
            let _e199 = inPositionTangentSpace_1;
            toLightVector_3 = (_e198 - _e199);
            let _e201 = toLightVector_3;
            unitToLightVector = normalize(_e201);
            let _e204 = light.attenuation;
            param_48 = _e204.xyz;
            let _e206 = toLightVector_3;
            param_49 = _e206;
            let _e207 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_48), (&param_49));
            attenuationFactor_3 = _e207;
            let _e208 = normal_2;
            param_50 = _e208;
            let _e209 = unitToLightVector;
            param_51 = _e209;
            let _e211 = light.color;
            param_52 = _e211.xyz;
            let _e213 = attenuationFactor_3;
            param_53 = _e213;
            let _e214 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_50), (&param_51), (&param_52), (&param_53));
            let _e215 = totalDiffuse;
            totalDiffuse = (_e215 + _e214);
            let _e217 = normal_2;
            param_54 = _e217;
            let _e218 = unitToLightVector;
            param_55 = _e218;
            let _e219 = unitToCameraVector;
            param_56 = _e219;
            let _e221 = light.color;
            param_57 = _e221.xyz;
            let _e223 = attenuationFactor_3;
            param_58 = _e223;
            let _e226 = uboFS.material.shineDamper;
            param_59 = _e226;
            let _e229 = uboFS.material.reflectivity;
            param_60 = _e229;
            let _e230 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_54), (&param_55), (&param_56), (&param_57), (&param_58), (&param_59), (&param_60));
            let _e231 = totalSpecular;
            totalSpecular = (_e231 + _e230);
            continue;
        } else {
            break;
        }
        continuing {
            let _e233 = i_3;
            i_3 = (_e233 + bitcast<u32>(1i));
        }
    }
    let _e236 = totalDiffuse;
    let _e237 = shadow_4;
    let _e241 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e236 * _e237) + vec3(_e241));
    let _e244 = totalSpecular;
    let _e245 = shadow_4;
    totalSpecular = (_e244 * _e245);
    let _e247 = totalDiffuse;
    let _e252 = textureColor;
    let _e254 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e247.x, _e247.y, _e247.z, 1f) * _e252) + vec4<f32>(_e254.x, _e254.y, _e254.z, 0f));
    let _e261 = uboFS.fogColor;
    let _e262 = _e261.xyz;
    let _e267 = baseResultColor;
    let _e268 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e262.x, _e262.y, _e262.z, 1f), _e267, vec4(_e268));
    let _e272 = uboFS.selected;
    if (_e272 != 0u) {
        let _e274 = resultColor;
        let _e276 = uboFS.selectedColor;
        resultColor = mix(_e274, _e276, vec4(0.5f));
    }
    let _e279 = resultColor;
    outColor = _e279;
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(7) inToLightVectorTangentSpace0_: vec3<f32>, @location(8) inToLightVectorTangentSpace1_: vec3<f32>, @location(9) inToLightVectorTangentSpace2_: vec3<f32>, @location(10) inToLightVectorTangentSpace3_: vec3<f32>, @location(11) inClipDistance: f32, @location(0) inTextureCoord: vec2<f32>, @location(6) inPositionTangentSpace: vec3<f32>, @location(3) inViewPosition: vec3<f32>, @location(2) inWorldPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(5) inToCameraVectorTangentSpace: vec3<f32>, @location(4) inVisibility: f32) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inToLightVectorTangentSpace0_1 = inToLightVectorTangentSpace0_;
    inToLightVectorTangentSpace1_1 = inToLightVectorTangentSpace1_;
    inToLightVectorTangentSpace2_1 = inToLightVectorTangentSpace2_;
    inToLightVectorTangentSpace3_1 = inToLightVectorTangentSpace3_;
    inClipDistance_1 = inClipDistance;
    inTextureCoord_1 = inTextureCoord;
    inPositionTangentSpace_1 = inPositionTangentSpace;
    inViewPosition_1 = inViewPosition;
    inWorldPosition_1 = inWorldPosition;
    inNormal_1 = inNormal;
    inToCameraVectorTangentSpace_1 = inToCameraVectorTangentSpace;
    inVisibility_1 = inVisibility;
    main_1();
    let _e27 = outColor;
    return _e27;
}
