#version 450
#extension GL_ARB_separate_shader_objects : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

layout(binding = 0) uniform sampler2D colorTex[MAX_VIEW_COUNT];
layout(binding = 1) uniform sampler2D depthTex[MAX_VIEW_COUNT];

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outFragColor;

void main()
{
#ifdef ENABLE_XR
    const int viewIndex = gl_ViewIndex;
#else
    const int viewIndex = 0;
#endif

    vec2 uv = inTextureCoord;

    vec4 color = texture(colorTex[viewIndex], uv);
    float depth = texture(depthTex[viewIndex], uv).r;

    outFragColor = color;
    gl_FragDepth = depth;
}