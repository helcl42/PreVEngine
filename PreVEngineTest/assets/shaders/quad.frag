#version 450

layout (binding = 0) uniform sampler2D texSampler;

layout (location = 0) in vec2 textureCoord;

layout (location = 0) out vec4 outFragColor;

float LinearizeDepth(float depth)
{
  float n = 0.01; // camera z near
  float f = 300.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));
}

void main() 
{
	float depth = texture(texSampler, textureCoord).r;
	outFragColor = vec4(vec3(1.0 - LinearizeDepth(depth)), 1.0);
}