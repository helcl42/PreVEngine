struct RaymarchToCloudResult {
    hit: bool,
    position: vec3<f32>,
    color: vec4<f32>,
}

struct UniformBufferObject {
    resolution: vec4<f32>,
    projectionMatrix: mat4x4<f32>,
    inverseProjectionMatrix: mat4x4<f32>,
    viewMatrix: mat4x4<f32>,
    inverseViewMatrix: mat4x4<f32>,
    lightColor: vec4<f32>,
    lightDirection: vec4<f32>,
    cameraPosition: vec4<f32>,
    baseCloudColor: vec4<f32>,
    skyColorBottom: vec4<f32>,
    skyColorTop: vec4<f32>,
    windDirection: vec4<f32>,
    worldOrigin: vec4<f32>,
    time: f32,
    coverageFactor: f32,
    cloudSpeed: f32,
    crispiness: f32,
    absorption: f32,
    curliness: f32,
    enablePowder: u32,
    densityFactor: f32,
    earthRadius: f32,
    sphereInnerRadius: f32,
    sphereOuterRadius: f32,
    cloudTopOffset: f32,
    maxDepth: f32,
}

@group(0) @binding(8) 
var<uniform> uboCS: UniformBufferObject;
@group(0) @binding(4) 
var perlinNoiseTex: texture_3d<f32>;
@group(0) @binding(5) 
var perlinNoiseSampler: sampler;
@group(0) @binding(6) 
var weatherTex: texture_2d<f32>;
@group(0) @binding(7) 
var weatherSampler: sampler;
var<private> gl_GlobalInvocationID_1: vec3<u32>;
@group(0) @binding(0) 
var outFragColor: texture_storage_2d<rgba8unorm,write>;
@group(0) @binding(1) 
var outBloom: texture_storage_2d<rgba8unorm,write>;
@group(0) @binding(2) 
var outAlphaness: texture_storage_2d<rgba8unorm,write>;
@group(0) @binding(3) 
var outCloudDistance: texture_storage_2d<r32float,write>;

fn ComputeDepth_u0028_mf44_u003b_mf44_u003b_vf3_u003b(projectionMatrix: ptr<function, mat4x4<f32>>, viewMatrix: ptr<function, mat4x4<f32>>, pointInWorldSpace: ptr<function, vec3<f32>>) -> f32 {
    var pInClipSpace: vec4<f32>;
    var pInNDC: vec3<f32>;
    var depth: f32;

    let _e142 = (*projectionMatrix);
    let _e143 = (*viewMatrix);
    let _e145 = (*pointInWorldSpace);
    pInClipSpace = ((_e142 * _e143) * vec4<f32>(_e145.x, _e145.y, _e145.z, 1f));
    let _e151 = pInClipSpace;
    let _e154 = pInClipSpace[3u];
    pInNDC = (_e151.xyz / vec3(_e154));
    let _e158 = pInNDC[2u];
    depth = _e158;
    let _e159 = depth;
    return _e159;
}

fn GetSunColor_u0028_vf3_u003b_f1_u003b(worldRayDirection: ptr<function, vec3<f32>>, powExp: ptr<function, f32>) -> vec3<f32> {
    var sun: f32;
    var col: vec3<f32>;

    let _e141 = uboCS.lightDirection;
    let _e143 = (*worldRayDirection);
    sun = clamp(dot(_e141.xyz, _e143), 0f, 1f);
    let _e146 = sun;
    let _e147 = (*powExp);
    col = (vec3<f32>(0.8f, 0.48f, 0.08f) * pow(_e146, _e147));
    let _e150 = col;
    return _e150;
}

fn GetPowder_u0028_f1_u003b(density: ptr<function, f32>) -> f32 {
    let _e137 = (*density);
    return (1f - exp((-2f * _e137)));
}

fn HG_u0028_f1_u003b_f1_u003b(sunDotRayDirection: ptr<function, f32>, g: ptr<function, f32>) -> f32 {
    var gg: f32;

    let _e139 = (*g);
    let _e140 = (*g);
    gg = (_e139 * _e140);
    let _e142 = gg;
    let _e144 = gg;
    let _e146 = (*g);
    let _e148 = (*sunDotRayDirection);
    return ((1f - _e142) / pow(((1f + _e144) - ((2f * _e146) * _e148)), 1.5f));
}

fn GetDensityForCloud_u0028_f1_u003b_f1_u003b(heightFraction: ptr<function, f32>, cloudType: ptr<function, f32>) -> f32 {
    var stratusFactor: f32;
    var stratoCumulusFactor: f32;
    var cumulusFactor: f32;
    var baseGradient: vec4<f32>;

    let _e142 = (*cloudType);
    stratusFactor = (1f - clamp((_e142 * 2f), 0f, 1f));
    let _e146 = (*cloudType);
    stratoCumulusFactor = (1f - (abs((_e146 - 0.5f)) * 2f));
    let _e151 = (*cloudType);
    cumulusFactor = (clamp((_e151 - 0.5f), 0f, 1f) * 2f);
    let _e155 = stratusFactor;
    let _e157 = stratoCumulusFactor;
    let _e160 = cumulusFactor;
    baseGradient = (((vec4<f32>(0f, 0.1f, 0.2f, 0.3f) * _e155) + (vec4<f32>(0.02f, 0.2f, 0.48f, 0.625f) * _e157)) + (vec4<f32>(0f, 0.1625f, 0.88f, 0.98f) * _e160));
    let _e164 = baseGradient[0u];
    let _e166 = baseGradient[1u];
    let _e167 = (*heightFraction);
    let _e170 = baseGradient[2u];
    let _e172 = baseGradient[3u];
    let _e173 = (*heightFraction);
    return (smoothstep(_e164, _e166, _e167) - smoothstep(_e170, _e172, _e173));
}

fn Remap_u0028_f1_u003b_f1_u003b_f1_u003b_f1_u003b_f1_u003b(originalValue: ptr<function, f32>, originalMin: ptr<function, f32>, originalMax: ptr<function, f32>, newMin: ptr<function, f32>, newMax: ptr<function, f32>) -> f32 {
    let _e141 = (*newMin);
    let _e142 = (*originalValue);
    let _e143 = (*originalMin);
    let _e145 = (*originalMax);
    let _e146 = (*originalMin);
    let _e149 = (*newMax);
    let _e150 = (*newMin);
    return (_e141 + (((_e142 - _e143) / (_e145 - _e146)) * (_e149 - _e150)));
}

fn GetUVProjection_u0028_vf3_u003b(inPos: ptr<function, vec3<f32>>) -> vec2<f32> {
    let _e137 = (*inPos);
    let _e140 = uboCS.sphereInnerRadius;
    return ((_e137.xz / vec2(_e140)) + vec2(0.5f));
}

fn GetEarthCenter_u0028_() -> vec3<f32> {
    var sphereCenter: vec3<f32>;

    let _e138 = uboCS.worldOrigin;
    let _e141 = uboCS.earthRadius;
    sphereCenter = (_e138.xyz + vec3<f32>(0f, -(_e141), 0f));
    let _e145 = sphereCenter;
    return _e145;
}

fn GetHeightFraction_u0028_vf3_u003b(inPos_1: ptr<function, vec3<f32>>) -> f32 {
    var sphereCenter_1: vec3<f32>;

    let _e138 = GetEarthCenter_u0028_();
    sphereCenter_1 = _e138;
    let _e139 = (*inPos_1);
    let _e140 = sphereCenter_1;
    let _e144 = uboCS.sphereInnerRadius;
    let _e147 = uboCS.sphereOuterRadius;
    let _e149 = uboCS.sphereInnerRadius;
    return ((length((_e139 - _e140)) - _e144) / (_e147 - _e149));
}

fn SampleCloudDensity_u0028_vf3_u003b_b1_u003b_f1_u003b(inPos_2: ptr<function, vec3<f32>>, expensive: ptr<function, bool>, lod: ptr<function, f32>) -> f32 {
    var heightFraction_1: f32;
    var param: vec3<f32>;
    var animation: vec3<f32>;
    var uv: vec2<f32>;
    var param_1: vec3<f32>;
    var movingUV: vec2<f32>;
    var param_2: vec3<f32>;
    var lowFrequencyNoise: vec4<f32>;
    var lowFreqFBM: f32;
    var baseCloud: f32;
    var param_3: f32;
    var param_4: f32;
    var param_5: f32;
    var param_6: f32;
    var param_7: f32;
    var density_1: f32;
    var param_8: f32;
    var param_9: f32;
    var weatherData: vec3<f32>;
    var cloudCoverage: f32;
    var baseCloudWithCoverage: f32;
    var param_10: f32;
    var param_11: f32;
    var param_12: f32;
    var param_13: f32;
    var param_14: f32;
    var erodeCloudNoise: vec3<f32>;
    var highFreqFBM: f32;
    var highFreqNoiseModifier: f32;
    var param_15: f32;
    var param_16: f32;
    var param_17: f32;
    var param_18: f32;
    var param_19: f32;

    let _e173 = (*inPos_2);
    param = _e173;
    let _e174 = GetHeightFraction_u0028_vf3_u003b((&param));
    heightFraction_1 = _e174;
    let _e175 = heightFraction_1;
    let _e177 = uboCS.windDirection;
    let _e181 = uboCS.cloudTopOffset;
    let _e184 = uboCS.windDirection;
    let _e187 = uboCS.time;
    let _e190 = uboCS.cloudSpeed;
    animation = (((_e177.xyz * _e175) * _e181) + ((_e184.xyz * _e187) * _e190));
    let _e193 = (*inPos_2);
    param_1 = _e193;
    let _e194 = GetUVProjection_u0028_vf3_u003b((&param_1));
    uv = _e194;
    let _e195 = (*inPos_2);
    let _e196 = animation;
    param_2 = (_e195 + _e196);
    let _e198 = GetUVProjection_u0028_vf3_u003b((&param_2));
    movingUV = _e198;
    let _e199 = heightFraction_1;
    let _e201 = heightFraction_1;
    if ((_e199 < 0f) || (_e201 > 1f)) {
        return 0f;
    }
    let _e204 = uv;
    let _e206 = uboCS.crispiness;
    let _e207 = (_e204 * _e206);
    let _e208 = heightFraction_1;
    let _e212 = (*lod);
    let _e213 = textureSampleLevel(perlinNoiseTex, perlinNoiseSampler, vec3<f32>(_e207.x, _e207.y, _e208), _e212);
    lowFrequencyNoise = _e213;
    let _e214 = lowFrequencyNoise;
    lowFreqFBM = dot(_e214.yzw, vec3<f32>(0.625f, 0.25f, 0.125f));
    let _e217 = lowFreqFBM;
    let _e221 = lowFrequencyNoise[0u];
    param_3 = _e221;
    param_4 = -((1f - _e217));
    param_5 = 1f;
    param_6 = 0f;
    param_7 = 1f;
    let _e222 = Remap_u0028_f1_u003b_f1_u003b_f1_u003b_f1_u003b_f1_u003b((&param_3), (&param_4), (&param_5), (&param_6), (&param_7));
    baseCloud = _e222;
    let _e223 = heightFraction_1;
    param_8 = _e223;
    param_9 = 1f;
    let _e224 = GetDensityForCloud_u0028_f1_u003b_f1_u003b((&param_8), (&param_9));
    density_1 = _e224;
    let _e225 = density_1;
    let _e226 = heightFraction_1;
    let _e228 = baseCloud;
    baseCloud = (_e228 * (_e225 / _e226));
    let _e230 = movingUV;
    let _e231 = textureSampleLevel(weatherTex, weatherSampler, _e230, 0f);
    weatherData = _e231.xyz;
    let _e234 = weatherData[0u];
    let _e236 = uboCS.coverageFactor;
    cloudCoverage = (_e234 * _e236);
    let _e238 = baseCloud;
    param_10 = _e238;
    let _e239 = cloudCoverage;
    param_11 = _e239;
    param_12 = 1f;
    param_13 = 0f;
    param_14 = 1f;
    let _e240 = Remap_u0028_f1_u003b_f1_u003b_f1_u003b_f1_u003b_f1_u003b((&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    baseCloudWithCoverage = _e240;
    let _e241 = cloudCoverage;
    let _e242 = baseCloudWithCoverage;
    baseCloudWithCoverage = (_e242 * _e241);
    let _e244 = (*expensive);
    if _e244 {
        let _e245 = movingUV;
        let _e247 = uboCS.crispiness;
        let _e248 = (_e245 * _e247);
        let _e249 = heightFraction_1;
        let _e254 = uboCS.curliness;
        let _e256 = (*lod);
        let _e257 = textureSampleLevel(perlinNoiseTex, perlinNoiseSampler, (vec3<f32>(_e248.x, _e248.y, _e249) * _e254), _e256);
        erodeCloudNoise = _e257.xyz;
        let _e259 = erodeCloudNoise;
        highFreqFBM = dot(_e259, vec3<f32>(0.625f, 0.25f, 0.125f));
        let _e261 = highFreqFBM;
        let _e262 = highFreqFBM;
        let _e264 = heightFraction_1;
        highFreqNoiseModifier = mix(_e261, (1f - _e262), clamp((_e264 * 10f), 0f, 1f));
        let _e268 = baseCloudWithCoverage;
        let _e269 = highFreqNoiseModifier;
        let _e270 = baseCloudWithCoverage;
        baseCloudWithCoverage = (_e268 - (_e269 * (1f - _e270)));
        let _e274 = baseCloudWithCoverage;
        let _e276 = highFreqNoiseModifier;
        param_15 = (_e274 * 2f);
        param_16 = (_e276 * 0.2f);
        param_17 = 1f;
        param_18 = 0f;
        param_19 = 1f;
        let _e278 = Remap_u0028_f1_u003b_f1_u003b_f1_u003b_f1_u003b_f1_u003b((&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
        baseCloudWithCoverage = _e278;
    }
    let _e279 = baseCloudWithCoverage;
    return clamp(_e279, 0f, 1f);
}

fn RaymarchToLight_u0028_vf3_u003b_f1_u003b_vf3_u003b_f1_u003b_f1_u003b(inPos_3: ptr<function, vec3<f32>>, stepSize: ptr<function, f32>, lightDir: ptr<function, vec3<f32>>, originalDensity: ptr<function, f32>, lightDotEye: ptr<function, f32>) -> f32 {
    var ds: f32;
    var rayStep: vec3<f32>;
    var sigmaDs: f32;
    var coneRadius: f32;
    var density_2: f32;
    var startPos: vec3<f32>;
    var T: f32;
    var i: i32;
    var pos: vec3<f32>;
    var indexable: array<vec3<f32>, 6>;
    var heightFraction_2: f32;
    var param_20: vec3<f32>;
    var cloudDensity: f32;
    var param_21: vec3<f32>;
    var param_22: bool;
    var param_23: f32;
    var Ti: f32;

    let _e158 = (*stepSize);
    ds = (_e158 * 6f);
    let _e160 = (*lightDir);
    let _e161 = ds;
    rayStep = (_e160 * _e161);
    let _e163 = ds;
    let _e166 = uboCS.absorption;
    sigmaDs = (-(_e163) * _e166);
    coneRadius = 1f;
    density_2 = 0f;
    let _e168 = (*inPos_3);
    startPos = _e168;
    T = 1f;
    i = 0i;
    loop {
        let _e169 = i;
        if (_e169 < 6i) {
            let _e171 = startPos;
            let _e172 = coneRadius;
            let _e173 = i;
            indexable = array<vec3<f32>, 6>(vec3<f32>(0.38051304f, 0.9245345f, -0.02111345f), vec3<f32>(-0.506258f, -0.03590792f, -0.8616342f), vec3<f32>(-0.32509217f, -0.9455744f, 0.01428793f), vec3<f32>(0.09026238f, -0.27376544f, 0.95755166f), vec3<f32>(0.28128597f, 0.4244364f, -0.8606579f), vec3<f32>(-0.16852403f, 0.14748697f, 0.97460103f));
            let _e175 = indexable[_e173];
            let _e177 = i;
            pos = (_e171 + ((_e175 * _e172) * f32(_e177)));
            let _e181 = pos;
            param_20 = _e181;
            let _e182 = GetHeightFraction_u0028_vf3_u003b((&param_20));
            heightFraction_2 = _e182;
            let _e183 = heightFraction_2;
            if (_e183 >= 0f) {
                let _e185 = density_2;
                let _e187 = i;
                let _e190 = pos;
                param_21 = _e190;
                param_22 = (_e185 > 0.3f);
                param_23 = f32((_e187 / 16i));
                let _e191 = SampleCloudDensity_u0028_vf3_u003b_b1_u003b_f1_u003b((&param_21), (&param_22), (&param_23));
                cloudDensity = _e191;
                let _e192 = cloudDensity;
                if (_e192 > 0f) {
                    let _e194 = cloudDensity;
                    let _e195 = sigmaDs;
                    Ti = exp((_e194 * _e195));
                    let _e198 = Ti;
                    let _e199 = T;
                    T = (_e199 * _e198);
                    let _e201 = cloudDensity;
                    let _e202 = density_2;
                    density_2 = (_e202 + _e201);
                }
            }
            let _e204 = rayStep;
            let _e205 = startPos;
            startPos = (_e205 + _e204);
            let _e207 = coneRadius;
            coneRadius = (_e207 + 0.16666667f);
            continue;
        } else {
            break;
        }
        continuing {
            let _e209 = i;
            i = (_e209 + 1i);
        }
    }
    let _e211 = T;
    return _e211;
}

fn CreateDefaultRaymarchToCloudResult_u0028_() -> RaymarchToCloudResult {
    var result: RaymarchToCloudResult;

    result.hit = false;
    result.position = vec3<f32>(1000000000000f, 1000000000000f, 1000000000000f);
    result.color = vec4<f32>(0f, 0f, 0f, 0f);
    let _e140 = result;
    return _e140;
}

fn RaymarchToCloud_u0028_vf3_u003b_vf3_u003b_vf3_u003b(startPos_1: ptr<function, vec3<f32>>, endPos: ptr<function, vec3<f32>>, bg: ptr<function, vec3<f32>>) -> RaymarchToCloudResult {
    var path: vec3<f32>;
    var pathLength: f32;
    var step_: f32;
    var dir: vec3<f32>;
    var stepVector: vec3<f32>;
    var fragCoord: vec2<u32>;
    var a: i32;
    var b: i32;
    var pos_1: vec3<f32>;
    var indexable_1: array<f32, 16>;
    var density_3: f32;
    var lightDotEye_1: f32;
    var T_1: f32;
    var sigmaDs_1: f32;
    var result_1: RaymarchToCloudResult;
    var i_1: i32;
    var densitySample: f32;
    var param_24: vec3<f32>;
    var param_25: bool;
    var param_26: f32;
    var ambientLight: vec3<f32>;
    var lightDensity: f32;
    var param_27: vec3<f32>;
    var param_28: f32;
    var param_29: vec3<f32>;
    var param_30: f32;
    var param_31: f32;
    var scattering: f32;
    var param_32: f32;
    var param_33: f32;
    var param_34: f32;
    var param_35: f32;
    var powderTerm: f32;
    var param_36: f32;
    var S: vec3<f32>;
    var dTrans: f32;
    var Sint: vec3<f32>;

    let _e176 = (*endPos);
    let _e177 = (*startPos_1);
    path = (_e176 - _e177);
    let _e179 = path;
    pathLength = length(_e179);
    let _e181 = pathLength;
    step_ = (_e181 / 64f);
    let _e183 = path;
    dir = normalize(_e183);
    let _e185 = dir;
    let _e186 = step_;
    stepVector = (_e185 * _e186);
    let _e188 = gl_GlobalInvocationID_1;
    fragCoord = _e188.xy;
    let _e191 = fragCoord[0u];
    let _e192 = bitcast<i32>(_e191);
    a = (_e192 - (i32(floor((f32(_e192) / f32(4i)))) * 4i));
    let _e201 = fragCoord[1u];
    let _e202 = bitcast<i32>(_e201);
    b = (_e202 - (i32(floor((f32(_e202) / f32(4i)))) * 4i));
    let _e210 = (*startPos_1);
    let _e211 = stepVector;
    let _e212 = a;
    let _e214 = b;
    indexable_1 = array<f32, 16>(0f, 0.5f, 0.125f, 0.625f, 0.75f, 0.25f, 0.875f, 0.375f, 0.1875f, 0.6875f, 0.0625f, 0.5625f, 0.9375f, 0.4375f, 0.8125f, 0.3125f);
    let _e217 = indexable_1[((_e212 * 4i) + _e214)];
    pos_1 = (_e210 + (_e211 * _e217));
    density_3 = 0f;
    let _e221 = uboCS.lightDirection;
    let _e224 = dir;
    lightDotEye_1 = dot(normalize(_e221.xyz), _e224);
    T_1 = 1f;
    let _e226 = step_;
    let _e229 = uboCS.densityFactor;
    sigmaDs_1 = (-(_e226) * _e229);
    let _e231 = CreateDefaultRaymarchToCloudResult_u0028_();
    result_1 = _e231;
    i_1 = 0i;
    loop {
        let _e232 = i_1;
        if (_e232 < 64i) {
            let _e234 = i_1;
            let _e237 = pos_1;
            param_24 = _e237;
            param_25 = true;
            param_26 = f32((_e234 / 16i));
            let _e238 = SampleCloudDensity_u0028_vf3_u003b_b1_u003b_f1_u003b((&param_24), (&param_25), (&param_26));
            densitySample = _e238;
            let _e239 = densitySample;
            if (_e239 > 0f) {
                let _e242 = result_1.hit;
                if !(_e242) {
                    let _e244 = pos_1;
                    result_1.position = _e244;
                    result_1.hit = true;
                }
                let _e248 = uboCS.baseCloudColor;
                ambientLight = _e248.xyz;
                let _e250 = step_;
                let _e252 = pos_1;
                param_27 = _e252;
                param_28 = (_e250 * 0.1f);
                let _e254 = uboCS.lightDirection;
                param_29 = _e254.xyz;
                let _e256 = densitySample;
                param_30 = _e256;
                let _e257 = lightDotEye_1;
                param_31 = _e257;
                let _e258 = RaymarchToLight_u0028_vf3_u003b_f1_u003b_vf3_u003b_f1_u003b_f1_u003b((&param_27), (&param_28), (&param_29), (&param_30), (&param_31));
                lightDensity = _e258;
                let _e259 = lightDotEye_1;
                param_32 = _e259;
                param_33 = -0.08f;
                let _e260 = HG_u0028_f1_u003b_f1_u003b((&param_32), (&param_33));
                let _e261 = lightDotEye_1;
                param_34 = _e261;
                param_35 = 0.08f;
                let _e262 = HG_u0028_f1_u003b_f1_u003b((&param_34), (&param_35));
                let _e263 = lightDotEye_1;
                scattering = mix(_e260, _e262, clamp(((_e263 * 0.5f) + 0.5f), 0f, 1f));
                let _e268 = scattering;
                scattering = max(_e268, 1f);
                powderTerm = 1f;
                let _e271 = uboCS.enablePowder;
                if (_e271 == 1u) {
                    let _e273 = densitySample;
                    param_36 = _e273;
                    let _e274 = GetPowder_u0028_f1_u003b((&param_36));
                    powderTerm = _e274;
                }
                let _e275 = ambientLight;
                let _e277 = (*bg);
                let _e280 = scattering;
                let _e282 = uboCS.lightColor;
                let _e285 = powderTerm;
                let _e286 = lightDensity;
                let _e291 = densitySample;
                S = ((mix(mix((_e275 * 1.8f), _e277, vec3(0.2f)), (_e282.xyz * _e280), vec3((_e285 * _e286))) * 0.6f) * _e291);
                let _e293 = densitySample;
                let _e294 = sigmaDs_1;
                dTrans = exp((_e293 * _e294));
                let _e297 = S;
                let _e298 = S;
                let _e299 = dTrans;
                let _e302 = densitySample;
                Sint = ((_e297 - (_e298 * _e299)) * (1f / _e302));
                let _e305 = T_1;
                let _e306 = Sint;
                let _e309 = result_1.color;
                let _e311 = (_e309.xyz + (_e306 * _e305));
                result_1.color[0u] = _e311.x;
                result_1.color[1u] = _e311.y;
                result_1.color[2u] = _e311.z;
                let _e321 = dTrans;
                let _e322 = T_1;
                T_1 = (_e322 * _e321);
            }
            let _e324 = T_1;
            if (_e324 <= 0.1f) {
                break;
            }
            let _e326 = stepVector;
            let _e327 = pos_1;
            pos_1 = (_e327 + _e326);
            continue;
        } else {
            break;
        }
        continuing {
            let _e329 = i_1;
            i_1 = (_e329 + 1i);
        }
    }
    let _e331 = T_1;
    result_1.color[3u] = (1f - _e331);
    let _e335 = result_1;
    return _e335;
}

fn ComputeFogAmount_u0028_vf3_u003b_f1_u003b(startPos_2: ptr<function, vec3<f32>>, factor: ptr<function, f32>) -> f32 {
    var sphereCenter_2: vec3<f32>;
    var dist: f32;
    var radius: f32;
    var alpha: f32;

    let _e142 = GetEarthCenter_u0028_();
    sphereCenter_2 = _e142;
    let _e143 = (*startPos_2);
    let _e145 = uboCS.cameraPosition;
    dist = length((_e143 - _e145.xyz));
    let _e151 = uboCS.cameraPosition[1u];
    let _e153 = sphereCenter_2[1u];
    radius = ((_e151 - _e153) * 0.3f);
    let _e156 = dist;
    let _e157 = radius;
    alpha = (_e156 / _e157);
    let _e159 = dist;
    let _e161 = alpha;
    let _e163 = (*factor);
    return (1f - exp(((-(_e159) * _e161) * _e163)));
}

fn RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b(rayStartPosition: ptr<function, vec3<f32>>, rayDirection: ptr<function, vec3<f32>>, sphereCenter_3: ptr<function, vec3<f32>>, radius_1: ptr<function, f32>, intersectionPoint: ptr<function, vec3<f32>>) -> bool {
    var radius2_: f32;
    var L: vec3<f32>;
    var a_1: f32;
    var b_1: f32;
    var c: f32;
    var discr: f32;
    var t: f32;

    let _e148 = (*radius_1);
    let _e149 = (*radius_1);
    radius2_ = (_e148 * _e149);
    let _e151 = (*rayStartPosition);
    let _e152 = (*sphereCenter_3);
    L = (_e151 - _e152);
    let _e154 = (*rayDirection);
    let _e155 = (*rayDirection);
    a_1 = dot(_e154, _e155);
    let _e157 = (*rayDirection);
    let _e158 = L;
    b_1 = (2f * dot(_e157, _e158));
    let _e161 = L;
    let _e162 = L;
    let _e164 = radius2_;
    c = (dot(_e161, _e162) - _e164);
    let _e166 = b_1;
    let _e167 = b_1;
    let _e169 = a_1;
    let _e171 = c;
    discr = ((_e166 * _e167) - ((4f * _e169) * _e171));
    let _e174 = discr;
    if (_e174 < 0f) {
        return false;
    }
    let _e176 = b_1;
    let _e178 = discr;
    t = max(0f, ((-(_e176) + sqrt(_e178)) / 2f));
    let _e183 = t;
    if (_e183 == 0f) {
        return false;
    }
    let _e185 = (*rayStartPosition);
    let _e186 = (*rayDirection);
    let _e187 = t;
    (*intersectionPoint) = (_e185 + (_e186 * _e187));
    return true;
}

fn GetCubeMapColor_u0028_vf3_u003b(worldRayDirection_1: ptr<function, vec3<f32>>) -> vec3<f32> {
    var transitionRatio: f32;
    var color: vec3<f32>;

    let _e140 = (*worldRayDirection_1)[1u];
    transitionRatio = smoothstep(-0.07f, 0.07f, _e140);
    let _e143 = uboCS.skyColorBottom;
    let _e146 = uboCS.skyColorTop;
    let _e148 = transitionRatio;
    color = mix(_e143.xyz, _e146.xyz, vec3(_e148));
    let _e151 = color;
    return _e151;
}

fn GetBackgroundColor_u0028_vf3_u003b(worldRayDirection_2: ptr<function, vec3<f32>>) -> vec4<f32> {
    var backgroundColor: vec3<f32>;
    var param_37: vec3<f32>;
    var sunColor: vec3<f32>;
    var param_38: vec3<f32>;
    var param_39: f32;
    var finalColor: vec3<f32>;

    let _e143 = (*worldRayDirection_2);
    param_37 = _e143;
    let _e144 = GetCubeMapColor_u0028_vf3_u003b((&param_37));
    backgroundColor = _e144;
    let _e145 = (*worldRayDirection_2);
    param_38 = _e145;
    param_39 = 350f;
    let _e146 = GetSunColor_u0028_vf3_u003b_f1_u003b((&param_38), (&param_39));
    sunColor = _e146;
    let _e147 = backgroundColor;
    let _e148 = sunColor;
    finalColor = (_e147 + _e148);
    let _e150 = finalColor;
    return vec4<f32>(_e150.x, _e150.y, _e150.z, 1f);
}

fn ComputeWorldSpaceDirection_u0028_vf4_u003b(viewSpaceDir: ptr<function, vec4<f32>>) -> vec3<f32> {
    var worldDir: vec3<f32>;

    let _e139 = uboCS.inverseViewMatrix;
    let _e140 = (*viewSpaceDir);
    worldDir = (_e139 * _e140).xyz;
    let _e143 = worldDir;
    return normalize(_e143);
}

fn ComputeViewSpaceDirection_u0028_vf4_u003b(clipSpaceDir: ptr<function, vec4<f32>>) -> vec4<f32> {
    var rayView: vec4<f32>;

    let _e139 = uboCS.inverseProjectionMatrix;
    let _e140 = (*clipSpaceDir);
    rayView = (_e139 * _e140);
    let _e142 = rayView;
    let _e143 = _e142.xy;
    return vec4<f32>(_e143.x, _e143.y, -1f, 0f);
}

fn ComputeClipSpaceDirection_u0028_vi2_u003b(fragCoord_1: ptr<function, vec2<i32>>) -> vec4<f32> {
    var normalizedFragCoord: vec2<f32>;
    var rayNds: vec2<f32>;

    let _e139 = (*fragCoord_1);
    let _e142 = uboCS.resolution;
    normalizedFragCoord = (vec2<f32>(_e139) / _e142.xy);
    let _e145 = normalizedFragCoord;
    rayNds = ((_e145 * 2f) - vec2(1f));
    let _e149 = rayNds;
    return vec4<f32>(_e149.x, _e149.y, 1f, 1f);
}

fn main_1() {
    var fragCoord_2: vec2<i32>;
    var clipDir: vec4<f32>;
    var param_40: vec2<i32>;
    var viewDir: vec4<f32>;
    var param_41: vec4<f32>;
    var worldDir_1: vec3<f32>;
    var param_42: vec4<f32>;
    var backgroundColor_1: vec4<f32>;
    var param_43: vec3<f32>;
    var sphereCenter_4: vec3<f32>;
    var startPos_3: vec3<f32>;
    var param_44: vec3<f32>;
    var param_45: vec3<f32>;
    var param_46: vec3<f32>;
    var param_47: f32;
    var param_48: vec3<f32>;
    var endPos_1: vec3<f32>;
    var param_49: vec3<f32>;
    var param_50: vec3<f32>;
    var param_51: vec3<f32>;
    var param_52: f32;
    var param_53: vec3<f32>;
    var fogRay: vec3<f32>;
    var param_54: vec3<f32>;
    var param_55: vec3<f32>;
    var param_56: vec3<f32>;
    var param_57: f32;
    var param_58: vec3<f32>;
    var hit: bool;
    var param_59: vec3<f32>;
    var param_60: vec3<f32>;
    var param_61: vec3<f32>;
    var param_62: f32;
    var param_63: vec3<f32>;
    var param_64: vec3<f32>;
    var param_65: vec3<f32>;
    var param_66: vec3<f32>;
    var param_67: f32;
    var param_68: vec3<f32>;
    var param_69: vec3<f32>;
    var param_70: vec3<f32>;
    var param_71: vec3<f32>;
    var param_72: f32;
    var param_73: vec3<f32>;
    var fogAmount: f32;
    var param_74: vec3<f32>;
    var param_75: f32;
    var raymarchResult: RaymarchToCloudResult;
    var param_76: vec3<f32>;
    var param_77: vec3<f32>;
    var param_78: vec3<f32>;
    var sun_1: f32;
    var s: vec3<f32>;
    var bloomColor: vec4<f32>;
    var param_79: vec3<f32>;
    var param_80: f32;
    var cloudAlphaness: f32;
    var alphaness: vec4<f32>;
    var fogAmount_1: f32;
    var param_81: vec3<f32>;
    var param_82: f32;
    var cloud: vec3<f32>;
    var fragColor: vec4<f32>;
    var depth_1: vec4<f32>;
    var param_83: mat4x4<f32>;
    var param_84: mat4x4<f32>;
    var param_85: vec3<f32>;
    var phi_1108_: bool;

    let _e203 = gl_GlobalInvocationID_1;
    fragCoord_2 = bitcast<vec2<i32>>(_e203.xy);
    let _e206 = fragCoord_2;
    param_40 = _e206;
    let _e207 = ComputeClipSpaceDirection_u0028_vi2_u003b((&param_40));
    clipDir = _e207;
    let _e208 = clipDir;
    param_41 = _e208;
    let _e209 = ComputeViewSpaceDirection_u0028_vf4_u003b((&param_41));
    viewDir = _e209;
    let _e210 = viewDir;
    param_42 = _e210;
    let _e211 = ComputeWorldSpaceDirection_u0028_vf4_u003b((&param_42));
    worldDir_1 = _e211;
    let _e212 = worldDir_1;
    param_43 = _e212;
    let _e213 = GetBackgroundColor_u0028_vf3_u003b((&param_43));
    backgroundColor_1 = _e213;
    let _e214 = GetEarthCenter_u0028_();
    sphereCenter_4 = _e214;
    let _e217 = uboCS.cameraPosition[1u];
    let _e219 = uboCS.sphereInnerRadius;
    let _e221 = uboCS.earthRadius;
    if (_e217 < (_e219 - _e221)) {
        let _e225 = uboCS.cameraPosition;
        param_44 = _e225.xyz;
        let _e227 = worldDir_1;
        param_45 = _e227;
        let _e228 = sphereCenter_4;
        param_46 = _e228;
        let _e230 = uboCS.sphereInnerRadius;
        param_47 = _e230;
        let _e231 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_44), (&param_45), (&param_46), (&param_47), (&param_48));
        let _e232 = param_48;
        startPos_3 = _e232;
        let _e234 = uboCS.cameraPosition;
        param_49 = _e234.xyz;
        let _e236 = worldDir_1;
        param_50 = _e236;
        let _e237 = sphereCenter_4;
        param_51 = _e237;
        let _e239 = uboCS.sphereOuterRadius;
        param_52 = _e239;
        let _e240 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_49), (&param_50), (&param_51), (&param_52), (&param_53));
        let _e241 = param_53;
        endPos_1 = _e241;
        let _e242 = startPos_3;
        fogRay = _e242;
    } else {
        let _e245 = uboCS.cameraPosition[1u];
        let _e247 = uboCS.sphereInnerRadius;
        let _e249 = uboCS.earthRadius;
        let _e251 = (_e245 > (_e247 - _e249));
        phi_1108_ = _e251;
        if _e251 {
            let _e254 = uboCS.cameraPosition[1u];
            let _e256 = uboCS.sphereOuterRadius;
            let _e258 = uboCS.earthRadius;
            phi_1108_ = (_e254 < (_e256 - _e258));
        }
        let _e262 = phi_1108_;
        if _e262 {
            let _e264 = uboCS.cameraPosition;
            startPos_3 = _e264.xyz;
            let _e267 = uboCS.cameraPosition;
            param_54 = _e267.xyz;
            let _e269 = worldDir_1;
            param_55 = _e269;
            let _e270 = sphereCenter_4;
            param_56 = _e270;
            let _e272 = uboCS.sphereOuterRadius;
            param_57 = _e272;
            let _e273 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_54), (&param_55), (&param_56), (&param_57), (&param_58));
            let _e274 = param_58;
            endPos_1 = _e274;
            let _e276 = uboCS.cameraPosition;
            param_59 = _e276.xyz;
            let _e278 = worldDir_1;
            param_60 = _e278;
            let _e279 = sphereCenter_4;
            param_61 = _e279;
            let _e281 = uboCS.sphereInnerRadius;
            param_62 = _e281;
            let _e282 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_59), (&param_60), (&param_61), (&param_62), (&param_63));
            let _e283 = param_63;
            fogRay = _e283;
            hit = _e282;
            let _e284 = hit;
            if !(_e284) {
                let _e286 = startPos_3;
                fogRay = _e286;
            }
        } else {
            let _e288 = uboCS.cameraPosition;
            param_64 = _e288.xyz;
            let _e290 = worldDir_1;
            param_65 = _e290;
            let _e291 = sphereCenter_4;
            param_66 = _e291;
            let _e293 = uboCS.sphereOuterRadius;
            param_67 = _e293;
            let _e294 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_64), (&param_65), (&param_66), (&param_67), (&param_68));
            let _e295 = param_68;
            startPos_3 = _e295;
            let _e297 = uboCS.cameraPosition;
            param_69 = _e297.xyz;
            let _e299 = worldDir_1;
            param_70 = _e299;
            let _e300 = sphereCenter_4;
            param_71 = _e300;
            let _e302 = uboCS.sphereInnerRadius;
            param_72 = _e302;
            let _e303 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_69), (&param_70), (&param_71), (&param_72), (&param_73));
            let _e304 = param_73;
            endPos_1 = _e304;
            let _e305 = startPos_3;
            fogRay = _e305;
        }
    }
    let _e306 = fogRay;
    param_74 = _e306;
    param_75 = 0.00006f;
    let _e307 = ComputeFogAmount_u0028_vf3_u003b_f1_u003b((&param_74), (&param_75));
    fogAmount = _e307;
    let _e308 = fogAmount;
    if (_e308 > 0.965f) {
        let _e310 = fragCoord_2;
        let _e311 = backgroundColor_1;
        textureStore(outFragColor, _e310, _e311);
        let _e312 = fragCoord_2;
        let _e313 = backgroundColor_1;
        textureStore(outBloom, _e312, _e313);
        let _e314 = fragCoord_2;
        textureStore(outAlphaness, _e314, vec4<f32>(0f, 0f, 0f, 0f));
        let _e315 = fragCoord_2;
        let _e317 = uboCS.maxDepth;
        textureStore(outCloudDistance, _e315, vec4(_e317));
        return;
    }
    let _e319 = startPos_3;
    param_76 = _e319;
    let _e320 = endPos_1;
    param_77 = _e320;
    let _e321 = backgroundColor_1;
    param_78 = _e321.xyz;
    let _e323 = RaymarchToCloud_u0028_vf3_u003b_vf3_u003b_vf3_u003b((&param_76), (&param_77), (&param_78));
    raymarchResult = _e323;
    let _e325 = raymarchResult.color;
    let _e329 = ((_e325.xyz * 1.8f) - vec3(0.1f));
    raymarchResult.color[0u] = _e329.x;
    raymarchResult.color[1u] = _e329.y;
    raymarchResult.color[2u] = _e329.z;
    let _e340 = raymarchResult.color;
    let _e342 = backgroundColor_1;
    let _e346 = raymarchResult.color[3u];
    let _e348 = fogAmount;
    let _e351 = mix(_e340.xyz, (_e342.xyz * _e346), vec3(clamp(_e348, 0f, 1f)));
    raymarchResult.color[0u] = _e351.x;
    raymarchResult.color[1u] = _e351.y;
    raymarchResult.color[2u] = _e351.z;
    let _e362 = uboCS.lightDirection;
    let _e364 = endPos_1;
    let _e365 = startPos_3;
    sun_1 = clamp(dot(_e362.xyz, normalize((_e364 - _e365))), 0f, 1f);
    let _e370 = sun_1;
    s = (vec3<f32>(0.8f, 0.32f, 0.16f) * pow(_e370, 256f));
    let _e373 = s;
    let _e376 = raymarchResult.color[3u];
    let _e379 = raymarchResult.color;
    let _e381 = (_e379.xyz + (_e373 * _e376));
    raymarchResult.color[0u] = _e381.x;
    raymarchResult.color[1u] = _e381.y;
    raymarchResult.color[2u] = _e381.z;
    let _e391 = backgroundColor_1;
    let _e395 = raymarchResult.color[3u];
    let _e399 = raymarchResult.color;
    let _e401 = ((_e391.xyz * (1f - _e395)) + _e399.xyz);
    backgroundColor_1[0u] = _e401.x;
    backgroundColor_1[1u] = _e401.y;
    backgroundColor_1[2u] = _e401.z;
    backgroundColor_1[3u] = 1f;
    let _e409 = worldDir_1;
    param_79 = _e409;
    param_80 = 128f;
    let _e410 = GetSunColor_u0028_vf3_u003b_f1_u003b((&param_79), (&param_80));
    let _e411 = (_e410 * 1.3f);
    bloomColor = vec4<f32>(_e411.x, _e411.y, _e411.z, 1f);
    let _e418 = raymarchResult.color[3u];
    cloudAlphaness = clamp(_e418, 0.2f, 1f);
    let _e420 = cloudAlphaness;
    alphaness = vec4<f32>(_e420, 0f, 0f, 1f);
    let _e422 = cloudAlphaness;
    if (_e422 > 0.1f) {
        let _e424 = startPos_3;
        param_81 = _e424;
        param_82 = 0.00003f;
        let _e425 = ComputeFogAmount_u0028_vf3_u003b_f1_u003b((&param_81), (&param_82));
        fogAmount_1 = _e425;
        let _e426 = bloomColor;
        let _e428 = fogAmount_1;
        cloud = mix(vec3<f32>(0f, 0f, 0f), _e426.xyz, vec3(clamp(_e428, 0f, 1f)));
        let _e432 = bloomColor;
        let _e434 = cloudAlphaness;
        let _e437 = cloud;
        let _e438 = ((_e432.xyz * (1f - _e434)) + _e437);
        bloomColor[0u] = _e438.x;
        bloomColor[1u] = _e438.y;
        bloomColor[2u] = _e438.z;
    }
    let _e445 = backgroundColor_1;
    let _e446 = _e445.xyz;
    fragColor = vec4<f32>(_e446.x, _e446.y, _e446.z, 1f);
    let _e452 = uboCS.projectionMatrix;
    param_83 = _e452;
    let _e454 = uboCS.viewMatrix;
    param_84 = _e454;
    let _e456 = raymarchResult.position;
    param_85 = _e456;
    let _e457 = ComputeDepth_u0028_mf44_u003b_mf44_u003b_vf3_u003b((&param_83), (&param_84), (&param_85));
    depth_1 = vec4<f32>(_e457, 0f, 0f, 0f);
    let _e459 = fragCoord_2;
    let _e460 = fragColor;
    textureStore(outFragColor, _e459, _e460);
    let _e461 = fragCoord_2;
    let _e462 = bloomColor;
    textureStore(outBloom, _e461, _e462);
    let _e463 = fragCoord_2;
    let _e464 = alphaness;
    textureStore(outAlphaness, _e463, _e464);
    let _e465 = fragCoord_2;
    let _e466 = depth_1;
    textureStore(outCloudDistance, _e465, _e466);
    return;
}

@compute @workgroup_size(16, 16, 1) 
fn main(@builtin(global_invocation_id) gl_GlobalInvocationID: vec3<u32>) {
    gl_GlobalInvocationID_1 = gl_GlobalInvocationID;
    main_1();
}
