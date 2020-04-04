#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 1) uniform UniformBufferObject {	
	vec4 color;
	vec4 selectedColor;
	uint selected;
} uboFS;

layout(location = 0) out vec4 outColor;

void main() 
{
	vec4 resultColor = uboFS.color;
	if(uboFS.selected != 0) {
		resultColor = uboFS.selectedColor;
	}
	outColor = resultColor;
}