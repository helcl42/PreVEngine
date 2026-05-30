@binding(0) @group(0) var outVolumeTexture_0 : texture_storage_3d<rgba8unorm, write>;

fn mod289_0( x_0 : vec4<f32>) -> vec4<f32>
{
    var _S1 : vec4<f32> = vec4<f32>(289.0f);
    return x_0 - floor(x_0 / _S1) * _S1;
}

fn permute_0( x_1 : vec4<f32>) -> vec4<f32>
{
    return mod289_0((x_1 * vec4<f32>(34.0f) + vec4<f32>(1.0f)) * x_1);
}

fn taylorInvSqrt_0( r_0 : vec4<f32>) -> vec4<f32>
{
    return vec4<f32>(1.79284286499023438f) - vec4<f32>(0.85373473167419434f) * r_0;
}

fn fade_0( t_0 : vec4<f32>) -> vec4<f32>
{
    return t_0 * t_0 * t_0 * (t_0 * (t_0 * vec4<f32>(vec4<i32>(i32(6))) - vec4<f32>(vec4<i32>(i32(15)))) + vec4<f32>(vec4<i32>(i32(10))));
}

fn glmPerlin4D_0( Position_0 : vec4<f32>,  rep_0 : vec4<f32>) -> f32
{
    var i_0 : i32;
    var _S2 : vec4<f32>;
    var _S3 : vec4<f32>;
    var _S4 : vec4<f32> = floor(Position_0);
    for(;;)
    {
        var result_0 : vec4<f32>;
        i_0 = i32(0);
        for(;;)
        {
            if(i_0 < i32(4))
            {
            }
            else
            {
                break;
            }
            result_0[i_0] = ((((_S4[i_0])) % ((rep_0[i_0]))));
            i_0 = i_0 + i32(1);
        }
        _S2 = result_0;
        break;
    }
    var _S5 : vec4<f32> = vec4<f32>(vec4<i32>(i32(1)));
    var _S6 : vec4<f32> = _S2 + _S5;
    for(;;)
    {
        var result_1 : vec4<f32>;
        i_0 = i32(0);
        for(;;)
        {
            if(i_0 < i32(4))
            {
            }
            else
            {
                break;
            }
            result_1[i_0] = ((((_S6[i_0])) % ((rep_0[i_0]))));
            i_0 = i_0 + i32(1);
        }
        _S3 = result_1;
        break;
    }
    var Pf0_0 : vec4<f32> = fract(Position_0);
    var Pf1_0 : vec4<f32> = Pf0_0 - _S5;
    var _S7 : f32 = _S2.x;
    var _S8 : f32 = _S3.x;
    var _S9 : f32 = _S2.y;
    var _S10 : f32 = _S3.y;
    var iw0_0 : vec4<f32> = vec4<f32>(_S2.w);
    var iw1_0 : vec4<f32> = vec4<f32>(_S3.w);
    var ixy_0 : vec4<f32> = permute_0(permute_0(vec4<f32>(_S7, _S8, _S7, _S8)) + vec4<f32>(_S9, _S9, _S10, _S10));
    var ixy0_0 : vec4<f32> = permute_0(ixy_0 + vec4<f32>(_S2.z));
    var ixy1_0 : vec4<f32> = permute_0(ixy_0 + vec4<f32>(_S3.z));
    var _S11 : vec4<f32> = vec4<f32>(vec4<i32>(i32(7)));
    var gx00_0 : vec4<f32> = permute_0(ixy0_0 + iw0_0) / _S11;
    var gy00_0 : vec4<f32> = floor(gx00_0) / _S11;
    var _S12 : vec4<f32> = vec4<f32>(vec4<i32>(i32(6)));
    var _S13 : vec4<f32> = vec4<f32>(0.5f);
    var gx00_1 : vec4<f32> = fract(gx00_0) - _S13;
    var gy00_1 : vec4<f32> = fract(gy00_0) - _S13;
    var gz00_0 : vec4<f32> = fract(floor(gy00_0) / _S12) - _S13;
    var _S14 : vec4<f32> = vec4<f32>(0.75f);
    var gw00_0 : vec4<f32> = _S14 - abs(gx00_1) - abs(gy00_1) - abs(gz00_0);
    var _S15 : vec4<f32> = vec4<f32>(0.0f);
    var sw00_0 : vec4<f32> = step(gw00_0, _S15);
    var _S16 : vec4<f32> = vec4<f32>(vec4<i32>(i32(0)));
    var gx00_2 : vec4<f32> = gx00_1 - sw00_0 * (step(_S16, gx00_1) - _S13);
    var gy00_2 : vec4<f32> = gy00_1 - sw00_0 * (step(_S16, gy00_1) - _S13);
    var gx01_0 : vec4<f32> = permute_0(ixy0_0 + iw1_0) / _S11;
    var gy01_0 : vec4<f32> = floor(gx01_0) / _S11;
    var gx01_1 : vec4<f32> = fract(gx01_0) - _S13;
    var gy01_1 : vec4<f32> = fract(gy01_0) - _S13;
    var gz01_0 : vec4<f32> = fract(floor(gy01_0) / _S12) - _S13;
    var gw01_0 : vec4<f32> = _S14 - abs(gx01_1) - abs(gy01_1) - abs(gz01_0);
    var sw01_0 : vec4<f32> = step(gw01_0, _S15);
    var gx01_2 : vec4<f32> = gx01_1 - sw01_0 * (step(_S16, gx01_1) - _S13);
    var gy01_2 : vec4<f32> = gy01_1 - sw01_0 * (step(_S16, gy01_1) - _S13);
    var gx10_0 : vec4<f32> = permute_0(ixy1_0 + iw0_0) / _S11;
    var gy10_0 : vec4<f32> = floor(gx10_0) / _S11;
    var gx10_1 : vec4<f32> = fract(gx10_0) - _S13;
    var gy10_1 : vec4<f32> = fract(gy10_0) - _S13;
    var gz10_0 : vec4<f32> = fract(floor(gy10_0) / _S12) - _S13;
    var gw10_0 : vec4<f32> = _S14 - abs(gx10_1) - abs(gy10_1) - abs(gz10_0);
    var sw10_0 : vec4<f32> = step(gw10_0, _S16);
    var gx10_2 : vec4<f32> = gx10_1 - sw10_0 * (step(_S16, gx10_1) - _S13);
    var gy10_2 : vec4<f32> = gy10_1 - sw10_0 * (step(_S16, gy10_1) - _S13);
    var gx11_0 : vec4<f32> = permute_0(ixy1_0 + iw1_0) / _S11;
    var gy11_0 : vec4<f32> = floor(gx11_0) / _S11;
    var gx11_1 : vec4<f32> = fract(gx11_0) - _S13;
    var gy11_1 : vec4<f32> = fract(gy11_0) - _S13;
    var gz11_0 : vec4<f32> = fract(floor(gy11_0) / _S12) - _S13;
    var gw11_0 : vec4<f32> = _S14 - abs(gx11_1) - abs(gy11_1) - abs(gz11_0);
    var sw11_0 : vec4<f32> = step(gw11_0, _S15);
    var gx11_2 : vec4<f32> = gx11_1 - sw11_0 * (step(_S16, gx11_1) - _S13);
    var gy11_2 : vec4<f32> = gy11_1 - sw11_0 * (step(_S16, gy11_1) - _S13);
    var g0000_0 : vec4<f32> = vec4<f32>(gx00_2.x, gy00_2.x, gz00_0.x, gw00_0.x);
    var g1000_0 : vec4<f32> = vec4<f32>(gx00_2.y, gy00_2.y, gz00_0.y, gw00_0.y);
    var g0100_0 : vec4<f32> = vec4<f32>(gx00_2.z, gy00_2.z, gz00_0.z, gw00_0.z);
    var g1100_0 : vec4<f32> = vec4<f32>(gx00_2.w, gy00_2.w, gz00_0.w, gw00_0.w);
    var g0010_0 : vec4<f32> = vec4<f32>(gx10_2.x, gy10_2.x, gz10_0.x, gw10_0.x);
    var g1010_0 : vec4<f32> = vec4<f32>(gx10_2.y, gy10_2.y, gz10_0.y, gw10_0.y);
    var g0110_0 : vec4<f32> = vec4<f32>(gx10_2.z, gy10_2.z, gz10_0.z, gw10_0.z);
    var g1110_0 : vec4<f32> = vec4<f32>(gx10_2.w, gy10_2.w, gz10_0.w, gw10_0.w);
    var g0001_0 : vec4<f32> = vec4<f32>(gx01_2.x, gy01_2.x, gz01_0.x, gw01_0.x);
    var g1001_0 : vec4<f32> = vec4<f32>(gx01_2.y, gy01_2.y, gz01_0.y, gw01_0.y);
    var g0101_0 : vec4<f32> = vec4<f32>(gx01_2.z, gy01_2.z, gz01_0.z, gw01_0.z);
    var g1101_0 : vec4<f32> = vec4<f32>(gx01_2.w, gy01_2.w, gz01_0.w, gw01_0.w);
    var g0011_0 : vec4<f32> = vec4<f32>(gx11_2.x, gy11_2.x, gz11_0.x, gw11_0.x);
    var g1011_0 : vec4<f32> = vec4<f32>(gx11_2.y, gy11_2.y, gz11_0.y, gw11_0.y);
    var g0111_0 : vec4<f32> = vec4<f32>(gx11_2.z, gy11_2.z, gz11_0.z, gw11_0.z);
    var g1111_0 : vec4<f32> = vec4<f32>(gx11_2.w, gy11_2.w, gz11_0.w, gw11_0.w);
    var norm00_0 : vec4<f32> = taylorInvSqrt_0(vec4<f32>(dot(g0000_0, g0000_0), dot(g0100_0, g0100_0), dot(g1000_0, g1000_0), dot(g1100_0, g1100_0)));
    var norm01_0 : vec4<f32> = taylorInvSqrt_0(vec4<f32>(dot(g0001_0, g0001_0), dot(g0101_0, g0101_0), dot(g1001_0, g1001_0), dot(g1101_0, g1101_0)));
    var norm10_0 : vec4<f32> = taylorInvSqrt_0(vec4<f32>(dot(g0010_0, g0010_0), dot(g0110_0, g0110_0), dot(g1010_0, g1010_0), dot(g1110_0, g1110_0)));
    var norm11_0 : vec4<f32> = taylorInvSqrt_0(vec4<f32>(dot(g0011_0, g0011_0), dot(g0111_0, g0111_0), dot(g1011_0, g1011_0), dot(g1111_0, g1111_0)));
    var _S17 : f32 = Pf1_0.x;
    var _S18 : f32 = Pf0_0.y;
    var _S19 : f32 = Pf0_0.z;
    var _S20 : f32 = Pf0_0.w;
    var _S21 : f32 = Pf0_0.x;
    var _S22 : f32 = Pf1_0.y;
    var _S23 : f32 = Pf1_0.z;
    var _S24 : f32 = Pf1_0.w;
    var fade_xyzw_0 : vec4<f32> = fade_0(Pf0_0);
    var _S25 : vec4<f32> = vec4<f32>(fade_xyzw_0.w);
    var n_zw_0 : vec4<f32> = mix(mix(vec4<f32>(dot(g0000_0 * vec4<f32>(norm00_0.x), Pf0_0), dot(g1000_0 * vec4<f32>(norm00_0.z), vec4<f32>(_S17, _S18, _S19, _S20)), dot(g0100_0 * vec4<f32>(norm00_0.y), vec4<f32>(_S21, _S22, _S19, _S20)), dot(g1100_0 * vec4<f32>(norm00_0.w), vec4<f32>(_S17, _S22, _S19, _S20))), vec4<f32>(dot(g0001_0 * vec4<f32>(norm01_0.x), vec4<f32>(_S21, _S18, _S19, _S24)), dot(g1001_0 * vec4<f32>(norm01_0.z), vec4<f32>(_S17, _S18, _S19, _S24)), dot(g0101_0 * vec4<f32>(norm01_0.y), vec4<f32>(_S21, _S22, _S19, _S24)), dot(g1101_0 * vec4<f32>(norm01_0.w), vec4<f32>(_S17, _S22, _S19, _S24))), _S25), mix(vec4<f32>(dot(g0010_0 * vec4<f32>(norm10_0.x), vec4<f32>(_S21, _S18, _S23, _S20)), dot(g1010_0 * vec4<f32>(norm10_0.z), vec4<f32>(_S17, _S18, _S23, _S20)), dot(g0110_0 * vec4<f32>(norm10_0.y), vec4<f32>(_S21, _S22, _S23, _S20)), dot(g1110_0 * vec4<f32>(norm10_0.w), vec4<f32>(_S17, _S22, _S23, _S20))), vec4<f32>(dot(g0011_0 * vec4<f32>(norm11_0.x), vec4<f32>(_S21, _S18, _S23, _S24)), dot(g1011_0 * vec4<f32>(norm11_0.z), vec4<f32>(_S17, _S18, _S23, _S24)), dot(g0111_0 * vec4<f32>(norm11_0.y), vec4<f32>(_S21, _S22, _S23, _S24)), dot(g1111_0 * vec4<f32>(norm11_0.w), Pf1_0)), _S25), vec4<f32>(fade_xyzw_0.z));
    var n_yzw_0 : vec2<f32> = mix(vec2<f32>(n_zw_0.x, n_zw_0.y), vec2<f32>(n_zw_0.z, n_zw_0.w), vec2<f32>(fade_xyzw_0.y));
    return 2.20000004768371582f * mix(n_yzw_0.x, n_yzw_0.y, fade_xyzw_0.x);
}

fn perlinNoise3D_0( pIn_0 : vec3<f32>,  frequency_0 : f32,  octaveCount_0 : i32) -> f32
{
    var oct_0 : i32 = i32(0);
    var _S26 : f32 = frequency_0;
    var weight_0 : f32 = 0.5f;
    var sum_0 : f32 = 0.0f;
    var weightSum_0 : f32 = 0.0f;
    for(;;)
    {
        if(oct_0 < octaveCount_0)
        {
        }
        else
        {
            break;
        }
        var _S27 : vec4<f32> = vec4<f32>(_S26);
        var sum_1 : f32 = sum_0 + glmPerlin4D_0(vec4<f32>(pIn_0.x, pIn_0.y, pIn_0.z, 0.0f) * _S27, _S27) * weight_0;
        var weightSum_1 : f32 = weightSum_0 + weight_0;
        var weight_1 : f32 = weight_0 * weight_0;
        var _S28 : f32 = _S26 * 2.0f;
        oct_0 = oct_0 + i32(1);
        _S26 = _S28;
        weight_0 = weight_1;
        sum_0 = sum_1;
        weightSum_0 = weightSum_1;
    }
    return max(min(sum_0 / weightSum_0, 1.0f), 0.0f);
}

fn hash_0( n_0 : i32) -> f32
{
    return fract(sin(f32(n_0) + 1.95099997520446777f) * 43758.546875f);
}

fn noise_0( x_2 : vec3<f32>) -> f32
{
    var p_0 : vec3<f32> = floor(x_2);
    var f_0 : vec3<f32> = fract(x_2);
    var f_1 : vec3<f32> = f_0 * f_0 * (vec3<f32>(3.0f) - vec3<f32>(2.0f) * f_0);
    var n_1 : f32 = p_0.x + p_0.y * 57.0f + 113.0f * p_0.z;
    var _S29 : f32 = f_1.x;
    var _S30 : f32 = f_1.y;
    return mix(mix(mix(hash_0(i32(n_1)), hash_0(i32(n_1 + 1.0f)), _S29), mix(hash_0(i32(n_1 + 57.0f)), hash_0(i32(n_1 + 58.0f)), _S29), _S30), mix(mix(hash_0(i32(n_1 + 113.0f)), hash_0(i32(n_1 + 114.0f)), _S29), mix(hash_0(i32(n_1 + 170.0f)), hash_0(i32(n_1 + 171.0f)), _S29), _S30), f_1.z);
}

fn cells_0( p_1 : vec3<f32>,  cellCount_0 : f32) -> f32
{
    var _S31 : vec3<f32>;
    var _S32 : vec3<f32> = p_1 * vec3<f32>(cellCount_0);
    var d_0 : f32 = 1.0e+10f;
    var xo_0 : i32 = i32(-1);
    for(;;)
    {
        if(xo_0 <= i32(1))
        {
        }
        else
        {
            break;
        }
        var yo_0 : i32 = i32(-1);
        for(;;)
        {
            if(yo_0 <= i32(1))
            {
            }
            else
            {
                break;
            }
            var d_1 : f32 = d_0;
            var zo_0 : i32 = i32(-1);
            for(;;)
            {
                if(zo_0 <= i32(1))
                {
                }
                else
                {
                    break;
                }
                var tp_0 : vec3<f32> = floor(_S32) + vec3<f32>(f32(xo_0), f32(yo_0), f32(zo_0));
                var _S33 : vec3<f32> = _S32 - tp_0;
                for(;;)
                {
                    var result_2 : vec3<f32>;
                    var i_1 : i32 = i32(0);
                    for(;;)
                    {
                        if(i_1 < i32(3))
                        {
                        }
                        else
                        {
                            break;
                        }
                        result_2[i_1] = ((((tp_0[i_1])) % ((cellCount_0))));
                        i_1 = i_1 + i32(1);
                    }
                    _S31 = result_2;
                    break;
                }
                var tp_1 : vec3<f32> = _S33 - vec3<f32>(noise_0(_S31));
                var _S34 : f32 = min(d_1, dot(tp_1, tp_1));
                var _S35 : i32 = zo_0 + i32(1);
                d_1 = _S34;
                zo_0 = _S35;
            }
            var _S36 : i32 = yo_0 + i32(1);
            d_0 = d_1;
            yo_0 = _S36;
        }
        xo_0 = xo_0 + i32(1);
    }
    return max(min(d_0, 1.0f), 0.0f);
}

fn worleyNoise3D_0( p_2 : vec3<f32>,  cellCount_1 : f32) -> f32
{
    return cells_0(p_2, cellCount_1);
}

fn remap_0( originalValue_0 : f32,  originalMin_0 : f32,  originalMax_0 : f32,  newMin_0 : f32,  newMax_0 : f32) -> f32
{
    return newMin_0 + (originalValue_0 - originalMin_0) / (originalMax_0 - originalMin_0) * (newMax_0 - newMin_0);
}

fn stackable3DNoise_0( pixel_0 : vec3<i32>) -> vec4<f32>
{
    var coord_0 : vec3<f32> = vec3<f32>(f32(pixel_0.x) / 128.0f, f32(pixel_0.y) / 128.0f, f32(pixel_0.z) / 128.0f);
    var worleyNoise1_0 : f32 = 1.0f - worleyNoise3D_0(coord_0, 32.0f);
    var _S37 : f32 = (1.0f - worleyNoise3D_0(coord_0, 8.0f)) * 0.625f;
    var _S38 : f32 = worleyNoise1_0 * 0.25f;
    var PerlinWorleyNoise_0 : f32 = remap_0(perlinNoise3D_0(coord_0, 8.0f, i32(3)), 0.0f, 1.0f, _S37 + _S38 + (1.0f - worleyNoise3D_0(coord_0, 56.0f)) * 0.125f, 1.0f);
    var w2_0 : f32 = 1.0f - worleyNoise3D_0(coord_0, 16.0f);
    var w4_0 : f32 = 1.0f - worleyNoise3D_0(coord_0, 64.0f);
    return vec4<f32>(PerlinWorleyNoise_0 * PerlinWorleyNoise_0, _S37 + w2_0 * 0.25f + worleyNoise1_0 * 0.125f, w2_0 * 0.625f + _S38 + w4_0 * 0.125f, worleyNoise1_0 * 0.75f + w4_0 * 0.25f);
}

@compute
@workgroup_size(4, 4, 4)
fn computeMain(@builtin(global_invocation_id) dispatchThreadID_0 : vec3<u32>)
{
    var _S39 : vec3<i32> = vec3<i32>(dispatchThreadID_0.xyz);
    textureStore((outVolumeTexture_0), (vec3<u32>(_S39)), (stackable3DNoise_0(_S39)));
    return;
}

