#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    uniform vec4 skyColorBottom;
    uniform vec4 skyColorTop;

    uniform vec4 lightDirection;
    uniform vec4 resolution;

    uniform mat4 inverseProjectionMatrix;
    uniform mat4 inverseViewMatrix;
} uboFS;

layout(location = 0) out vec4 outColor;

vec3 RaySphereIntersectionSkyMap(in vec3 rayDirection, in float radius)
{	
	const vec3 sphereCenter = vec3(0.0);
	const float radius2 = radius * radius;

	vec3 L = -sphereCenter;
	float a = dot(rayDirection, rayDirection);
	float b = 2.0 * dot(rayDirection, L);
	float c = dot(L, L) - radius2;

	float discr = b * b - 4.0 * a * c;
	float t = max(0.0, (-b + sqrt(discr)) / 2.0);

	return rayDirection * t;
}

vec3 ComputeClipSpaceCoord(in ivec2 fragCoord)
{
	vec2 rayNds = 2.0 * vec2(fragCoord.xy) / uboFS.resolution.xy - 1.0;
	return vec3(rayNds, 1.0);
}

vec3 GetSunColor(in vec3 rayDirection, in float powExp)
{
	float sun = clamp(dot(-uboFS.lightDirection.xyz, rayDirection), 0.0, 1.0);
	vec3 col = 0.8 * vec3(1.0, 0.6, 0.1) * pow(sun, powExp);
	return col;
}

vec4 GetCubeMapColor(in vec3 rayDirection, in vec3 endPos)
{
	vec3 color = mix(uboFS.skyColorBottom.xyz, uboFS.skyColorTop.xyz, clamp(1.0 - exp(8.5 - 17.0 * clamp(normalize(rayDirection).y * 0.5 + 0.5, 0.0, 1.0)), 0.0, 1.0));
	color += GetSunColor(rayDirection, 350.0);
	return vec4(color, 1.0);
}

void main()
{    
	ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	vec4 rayClip = vec4(ComputeClipSpaceCoord(fragCoord), 1.0);
	vec4 rayView = uboFS.inverseProjectionMatrix * rayClip;
	rayView = vec4(rayView.xy, -1.0, 0.0);

	vec3 worldDir = (uboFS.inverseViewMatrix * rayView).xyz;
	worldDir = normalize(worldDir);

	vec3 cubeMapEndPos = RaySphereIntersectionSkyMap(worldDir, 0.5);
	vec4 background = GetCubeMapColor(worldDir, cubeMapEndPos);
	
	outColor = vec4(background.rgb, 1.0);
}