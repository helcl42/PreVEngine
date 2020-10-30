#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <prev/common/Common.h>

namespace prev_test {
constexpr auto TAG_LIGHT{ "Light" };
constexpr auto TAG_MAIN_LIGHT{ "MainLight" };
constexpr auto TAG_SHADOW{ "Shadow" };
constexpr auto TAG_CAMERA{ "Camera" };
constexpr auto TAG_MAIN_CAMERA{ "MainCamera" };
constexpr auto TAG_PLAYER{ "Player" };

constexpr auto TAG_TRANSFORM_COMPONENT{ "TransformComponent" };
constexpr auto TAG_RAYCASTER_COMPONENT{ "RayCasterComponent" };
constexpr auto TAG_RENDER_COMPONENT{ "RenderComponent" };
constexpr auto TAG_RENDER_NORMAL_MAPPED_COMPONENT{ "RenderNormalMappedComponent" };
constexpr auto TAG_RENDER_PARALLAX_MAPPED_COMPONENT{ "RenderParallaxMappedRenderComponent" };
constexpr auto TAG_RENDER_CONE_STEP_MAPPED_COMPONENT{ "RenderConeStepMappedRenderComponent" };
constexpr auto TAG_ANIMATION_RENDER_COMPONENT{ "AnimationRenderComponent" };
constexpr auto TAG_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT{ "AnimationNormalMappedRenderComponent" };
constexpr auto TAG_ANIMATION_PARALLAX_MAPPED_RENDER_COMPONENT{ "AnimationParallaxMappedRenderComponent" };
constexpr auto TAG_ANIMATION_CONE_STEP_MAPPED_RENDER_COMPONENT{ "AnimationConeStepMappedRenderComponent" };
constexpr auto TAG_FONT_RENDER_COMPONENT{ "FontRenderComponent" };
constexpr auto TAG_SKYBOX_RENDER_COMPONENT{ "SkyBoxRenderComponent" };
constexpr auto TAG_TERRAIN_MANAGER_COMPONENT{ "TerrainManagerComponent" };
constexpr auto TAG_TERRAIN_RENDER_COMPONENT{ "TerrainRenderComponent" };
constexpr auto TAG_TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT{ "TerrainNormalMappedRenderComponent" };
constexpr auto TAG_TERRAIN_PARALLAX_MAPPED_RENDER_COMPONENT{ "TerrainParallaxMappedRenderComponent" };
constexpr auto TAG_TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT{ "TerrainConeStepMappedRenderComponent" };
constexpr auto TAG_CAMERA_COMPONENT{ "CameraComponent" };
constexpr auto TAG_SHADOWS_COMPONENT{ "ShadowsComponent" };
constexpr auto TAG_LIGHT_COMPONENT{ "LightComponent" };
constexpr auto TAG_WATER_REFLECTION_RENDER_COMPONENT{ "WaterReflectionRenderComponent" };
constexpr auto TAG_WATER_REFRACTION_RENDER_COMPONENT{ "WaterRefractionRenderComponent" };
constexpr auto TAG_WATER_RENDER_COMPONENT{ "WaterRenderComponent" };
constexpr auto TAG_LENS_FLARE_RENDER_COMPONENT{ "LensFlareRenderComponent" };
constexpr auto TAG_SUN_RENDER_COMPONENT{ "SunRenderComponent" };
constexpr auto TAG_BOUNDING_VOLUME_COMPONENT{ "BoundingVolumeComponent" };
constexpr auto TAG_SELECTABLE_COMPONENT{ "SelectableComponent" };
constexpr auto TAG_PARTICLE_SYSTEM_COMPONENT{ "ParticleSystemComponent" };
constexpr auto TAG_CLOUDS_COMPONENT{ "CloudsComponent" };
constexpr auto TAG_SKY_RENDER_COMPONENT{ "SkyRenderComponent" };
constexpr auto TAG_TIME_COMPONENT{ "TimeComponent" };
} // namespace prev_test

#endif