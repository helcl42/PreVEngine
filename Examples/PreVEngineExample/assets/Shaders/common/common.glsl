
float GetVisibility(in vec3 viewPosition, in float gradient, in float density)
{
	float vertexToCameraDistance = length(viewPosition);
	float visibility = exp(-pow(vertexToCameraDistance * density, gradient));
	return clamp(visibility, 0.0, 1.0);
}

mat3 CreateTBNMatrix(in mat3 transform, in vec3 normal, in vec3 tangent, in vec3 biTangent)
{
	vec3 T = normalize(transform * tangent);
	vec3 B = normalize(transform * biTangent);
	vec3 N = normalize(transform * normal);
	mat3 TBN = transpose(mat3(T, B, N));
	return TBN;
}