#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {
    mat4 modelMatrix;

    mat4 viewMatrix;
    
	mat4 projectionMatrix;
} uboGS;

const float SIZE = 0.2;
const vec3 LIGHT_DIRECTION = normalize(vec3(0.4, -1.0, 0.8));
const float AMBIENT_LIGHT = 0.3;

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

layout(location = 0) in vec3 intColor[];

layout(location = 0) out vec3 finalColor;

void CreateVertex(in vec3 offset, in vec3 faceColor)
{
	vec4 actualOffset = vec4(offset * SIZE, 0.0);
	vec4 worldPosition = gl_in[0].gl_Position + actualOffset;
	gl_Position = uboGS.projectionMatrix * uboGS.viewMatrix * uboGS.modelMatrix * worldPosition;
	finalColor = faceColor;
	EmitVertex();
}

vec3 CalculateLightColor(in vec3 normal)
{
	float brightness = max(dot(-LIGHT_DIRECTION, normal), AMBIENT_LIGHT);
	return intColor[0] * brightness;
}

void main()
{
	vec3 faceColor = CalculateLightColor(vec3(0.0, 0.0, 1.0));
	CreateVertex(vec3(-1.0, 1.0, 1.0), faceColor);
	CreateVertex(vec3(-1.0, -1.0, 1.0), faceColor);
	CreateVertex(vec3(1.0, 1.0, 1.0), faceColor);
	CreateVertex(vec3(1.0, -1.0, 1.0), faceColor);
	EndPrimitive();

	faceColor = CalculateLightColor(vec3(1.0, 0.0, 0.0));
	CreateVertex(vec3(1.0, 1.0, 1.0), faceColor);
	CreateVertex(vec3(1.0, -1.0, 1.0), faceColor);
	CreateVertex(vec3(1.0, 1.0, -1.0), faceColor);
	CreateVertex(vec3(1.0, -1.0, -1.0), faceColor);
	EndPrimitive();

	faceColor = CalculateLightColor(vec3(0.0, 0.0, -0.0));
	CreateVertex(vec3(1.0, 1.0, -1.0), faceColor);
	CreateVertex(vec3(1.0, -1.0, -1.0), faceColor);
	CreateVertex(vec3(-1.0, 1.0, -1.0), faceColor);
	CreateVertex(vec3(-1.0, -1.0, -1.0), faceColor);
	EndPrimitive();

	faceColor = CalculateLightColor(vec3(-1.0, 0.0, 0.0));
	CreateVertex(vec3(-1.0, 1.0, -1.0), faceColor);
	CreateVertex(vec3(-1.0, -1.0, -1.0), faceColor);
	CreateVertex(vec3(-1.0, 1.0, 1.0), faceColor);
	CreateVertex(vec3(-1.0, -1.0, 1.0), faceColor);
	EndPrimitive();

	faceColor = CalculateLightColor(vec3(0.0, 1.0, 0.0));
	CreateVertex(vec3(1.0, 1.0, 1.0), faceColor);
	CreateVertex(vec3(1.0, 1.0, -1.0), faceColor);
	CreateVertex(vec3(-1.0, 1.0, 1.0), faceColor);
	CreateVertex(vec3(-1.0, 1.0, -1.0), faceColor);
	EndPrimitive();

	faceColor = CalculateLightColor(vec3(0.0, -1.0, 0.0));
	CreateVertex(vec3(-1.0, -1.0, 1.0), faceColor);
	CreateVertex(vec3(-1.0, -1.0, -1.0), faceColor);
	CreateVertex(vec3(1.0, -1.0, 1.0), faceColor);
	CreateVertex(vec3(1.0, -1.0, -1.0), faceColor);
	EndPrimitive();
}