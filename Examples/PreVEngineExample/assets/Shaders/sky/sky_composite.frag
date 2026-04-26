#version 450
#extension GL_ARB_separate_shader_objects : enable
#ifdef ENABLE_XR
#extension GL_EXT_multiview : enable
#endif

#ifdef ENABLE_XR
layout(binding = 0) uniform texture2D colorTex[MAX_VIEW_COUNT];
layout(binding = 1) uniform sampler colorSampler;
layout(binding = 2) uniform texture2D depthTex[MAX_VIEW_COUNT];
layout(binding = 3) uniform sampler depthSampler;
#else
layout(binding = 0) uniform texture2D colorTex;
layout(binding = 1) uniform sampler colorSampler;
layout(binding = 2) uniform texture2D depthTex;
layout(binding = 3) uniform sampler depthSampler;
#endif

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outFragColor;

void main()
{
    vec2 uv = inTextureCoord;

#ifdef ENABLE_XR
    vec4 color = texture(sampler2D(colorTex[gl_ViewIndex], colorSampler), uv);
    float depth = texture(sampler2D(depthTex[gl_ViewIndex], depthSampler), uv).r;
#else
    vec4 color = texture(sampler2D(colorTex, colorSampler), uv);
    float depth = texture(sampler2D(depthTex, depthSampler), uv).r;
#endif

    outFragColor = color;
    gl_FragDepth = depth;
}