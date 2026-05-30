// Common functions module
module common;

public float GetVisibility(float3 viewPosition, float gradient, float density)
{
    float vertexToCameraDistance = length(viewPosition);
    float visibility = exp(-pow(vertexToCameraDistance * density, gradient));
    return clamp(visibility, 0.0, 1.0);
}

public float3x3 CreateTBNMatrix(float3x3 transform, float3 normal, float3 tangent, float3 biTangent)
{
    float3 T = normalize(mul(transform, tangent));
    float3 B = normalize(mul(transform, biTangent));
    float3 N = normalize(mul(transform, normal));
    float3x3 TBN = float3x3(T, B, N);
    return TBN;
}
