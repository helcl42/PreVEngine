// parallax mapping client 

const float parallaxBias = 0.0;

vec2 ParallaxMappingInternal(in sampler2D heightMapSampler, in float heightScale, in vec2 uv, in vec3 viewDir)
{
	float height = 1.0 - texture(heightMapSampler, uv).r;
	vec2 p = viewDir.xy * (height * (heightScale * 0.5) + parallaxBias) / viewDir.z;
	return uv - p;  
}

vec2 SteepParallaxMappingInternal(in sampler2D heightMapSampler, in float heightScale, in uint numLayers, in vec2 uv, in vec3 viewDir)
{
	float layerDepth = 1.0 / numLayers;
	float currLayerDepth = 0.0;
	vec2 deltaUV = viewDir.xy * heightScale / (viewDir.z * numLayers);
	vec2 currUV = uv;
	float height = 1.0 - texture(heightMapSampler, currUV).r;
	for (int i = 0; i < numLayers; i++) 
	{
		currLayerDepth += layerDepth;
		currUV -= deltaUV;
		height = 1.0 - texture(heightMapSampler, currUV).r;
		if (height < currLayerDepth)
		{
			break;
		}
	}
	return currUV;
}

vec2 ParallaxOcclusionMappingInternal(in sampler2D heightMapSampler, in float heightScale, in uint numLayers, in vec2 uv, in vec3 viewDir)
{
	float layerDepth = 1.0 / numLayers;
	float currLayerDepth = 0.0;
	vec2 deltaUV = viewDir.xy * heightScale / (viewDir.z * numLayers);
	vec2 currUV = uv;
	float height = 1.0 - texture(heightMapSampler, currUV).r;
	for (int i = 0; i < numLayers; i++) 
	{
		currLayerDepth += layerDepth;
		currUV -= deltaUV;
		height = 1.0 - texture(heightMapSampler, currUV).r;
		if (height < currLayerDepth) 
		{
			break;
		}
	}
	vec2 prevUV = currUV + deltaUV;
	float nextDepth = height - currLayerDepth;
	float prevDepth = 1.0 - texture(heightMapSampler, prevUV).r - currLayerDepth + layerDepth;
	return mix(currUV, prevUV, nextDepth / (nextDepth - prevDepth));
}

vec2 ParallaxMapping(in uint mode, in sampler2D heightMapSampler, in float heightScale, in uint numLayers, in vec2 uv, in vec3 viewDir)
{
	vec2 resultUV;
	switch(mode) 
	{
		case 1:
			resultUV = ParallaxMappingInternal(heightMapSampler, heightScale, uv, viewDir);
			break;
		case 2:
			resultUV = SteepParallaxMappingInternal(heightMapSampler, heightScale, numLayers, uv, viewDir);
			break;
		case 3:
			resultUV = ParallaxOcclusionMappingInternal(heightMapSampler, heightScale, numLayers, uv, viewDir);
			break;
		default:
			resultUV = uv;
			break;
	}
	return resultUV;
}
