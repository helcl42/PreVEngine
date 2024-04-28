// normal mapping client code

vec3 NormalMapping(in sampler2D normalMapSampler, in vec2 uv)
{
    return normalize(2.0 * normalize(texture(normalMapSampler, uv).xyz) - 1.0);
}

vec3 NormalMapping(in sampler2D normalMapSampler, in vec2 uv, in vec2 ddx, in vec2 ddy)
{
    return normalize(2.0 * normalize(textureGrad(normalMapSampler, uv, ddx, ddy).xyz) - 1.0);
}