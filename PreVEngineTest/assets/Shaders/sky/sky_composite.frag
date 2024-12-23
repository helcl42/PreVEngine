#version 450
#extension GL_ARB_separate_shader_objects : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

#ifdef ENABLE_XR
layout(binding = 0) uniform sampler2D colorTex[MAX_VIEW_COUNT];
layout(binding = 1) uniform sampler2D depthTex[MAX_VIEW_COUNT];
#else
layout(binding = 0) uniform sampler2D colorTex;
layout(binding = 1) uniform sampler2D depthTex;
#endif

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outFragColor;

void main()
{
    vec2 uv = inTextureCoord;

#ifdef ENABLE_XR
    vec4 color = texture(colorTex[gl_ViewIndex], uv);
    float depth = texture(depthTex[gl_ViewIndex], uv).r;
#else
    vec4 color = texture(colorTex, uv);
    float depth = texture(depthTex, uv).r;
#endif

    outFragColor = color;
    gl_FragDepth = depth;
}