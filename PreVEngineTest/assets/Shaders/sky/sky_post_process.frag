#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    vec4 resolution;
    vec4 lightPosition;
    uint enableGodRays;
    float lightDotCameraFront;
} uboFS;

layout(binding = 1) uniform sampler2D skyTex;
layout(binding = 2) uniform sampler2D bloomTex;

layout(location = 0) in vec2 inTextureCoord;

layout(location = 0) out vec4 outFragColor;


#define  BLUR_OFFSET_X  1.0 / uboFS.resolution.x
#define  BLUR_OFFSET_Y  1.0 / uboFS.resolution.y

vec4 GaussianBlur(in sampler2D tex, in vec2 uv)
{
    vec2 offsets[9] = vec2[](
        vec2(-BLUR_OFFSET_X,  BLUR_OFFSET_Y),
        vec2(0.0f, BLUR_OFFSET_Y),
        vec2(BLUR_OFFSET_X, BLUR_OFFSET_Y),
        vec2(BLUR_OFFSET_X, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(BLUR_OFFSET_X, 0.0f),
        vec2(BLUR_OFFSET_X, -BLUR_OFFSET_Y),
        vec2(0.0f, -BLUR_OFFSET_Y),
        vec2(BLUR_OFFSET_X, -BLUR_OFFSET_Y)
    );

	float kernel[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	);

    vec4 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
		vec4 pixel = texture(tex, uv.st + offsets[i]);
        sampleTex[i] = pixel;
    }

    vec4 col = vec4(0.0);
    for(int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * kernel[i];
    }
    return col;
}

void main()
{
    vec2 uv = inTextureCoord;

	vec4 fragColor = GaussianBlur(skyTex, uv);

	// RADIAL BLUR + CREPUSCOLAR RAYS
	bvec2 lowerLimit = greaterThan(uboFS.lightPosition.xy, vec2(0.0));
	bvec2 upperLimit = lessThan(uboFS.lightPosition.xy, vec2(1.0));
	if(uboFS.lightDotCameraFront > 0.0 && uboFS.enableGodRays != 0)
	{
        // Radial blur factors
        float decay = 0.98;
        float density = 0.9;
        float weight = 0.07;
        float exposure = 0.45;

        // Light offset
        vec3 l = vec3(uboFS.lightPosition.xy, 0.5);

        const int SAMPLES = 64;
        float illuminationDecay = 1.0;

        vec2 textureCoord = uv;
        vec2 textureCoordDelta = textureCoord - uboFS.lightPosition.xy;
        textureCoordDelta *= density / float(SAMPLES);

        vec3 colRays = GaussianBlur(bloomTex, uv).rgb * 0.4;
        for(int i = 0; i < SAMPLES; i++)
        {
            uv -= textureCoordDelta;
            colRays += texture(bloomTex, uv).rgb * illuminationDecay * weight;
            illuminationDecay *= decay;
        }

        vec3 colorWithRays = fragColor.rgb + (smoothstep(0.0, 1.0, colRays) * exposure);
        fragColor.rgb = mix(fragColor.rgb, colorWithRays * 0.9, uboFS.lightDotCameraFront * uboFS.lightDotCameraFront);
	}

    outFragColor = fragColor;
}