#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D colorTex;
layout(binding = 1) uniform sampler2D depthTex;

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outFragColor;

void main()
{
    vec2 uv = inTextureCoord;

    vec4 color = texture(colorTex, uv);
    float depth = texture(depthTex, uv).r;

    outFragColor = color;
    gl_FragDepth = depth;
}