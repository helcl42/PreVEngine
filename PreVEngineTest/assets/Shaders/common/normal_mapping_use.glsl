// normal mapping client code

vec3 NormalMapping(in sampler2D normalMapSampler, in vec2 uv)
{
    return normalize(2.0 * texture(normalMapSampler, uv).xyz - 1.0);
}