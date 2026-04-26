// normal mapping client code

vec3 NormalMapping(in texture2D normalMapTexture, in sampler normalMapSampler, in vec2 uv)
{
    return normalize(2.0 * normalize(texture(sampler2D(normalMapTexture, normalMapSampler), uv).xyz) - 1.0);
}

vec3 NormalMapping(in texture2D normalMapTexture, in sampler normalMapSampler, in vec2 uv, in vec2 ddx, in vec2 ddy)
{
    return normalize(2.0 * normalize(textureGrad(sampler2D(normalMapTexture, normalMapSampler), uv, ddx, ddy).xyz) - 1.0);
}