#include "MasterRenderer.h"
#include "CommandBuffersGroupFactory.h"

#include "../../common/intersection/Frustum.h"
#include "../../component/camera/ICameraComponent.h"
#include "../../component/common/IOffScreenRenderPassComponent.h"
#include "../../component/shadow/IShadowsComponent.h"
#include "../../component/water/IWaterComponent.h"
#include "../../component/water/WaterCommon.h"
#include "animation/AnimationConeStepMappedRenderer.h"
#include "animation/AnimationNormalMappedRenderer.h"
#include "animation/AnimationParallaxMappedRenderer.h"
#include "animation/AnimationRenderer.h"
#include "animation/AnimationTexturelessRenderer.h"
#include "debug/BoundingVolumeDebugRenderer.h"
#include "debug/RayCastDebugRenderer.h"
#include "debug/SelectionDebugRenderer.h"
#include "debug/ShadowMapDebugRenderer.h"
#include "debug/TextureDebugRenderer.h"
#include "font/FontRenderer.h"
#include "normal/ConeStepMappedRenderer.h"
#include "normal/DefaultRenderer.h"
#include "normal/NormalMappedRenderer.h"
#include "normal/ParallaxMappedRenderer.h"
#include "normal/TexturelessRenderer.h"
#include "particle/ParticlesRenderer.h"
#include "shadow/AnimationBumpMappedShadowsRenderer.h"
#include "shadow/AnimationShadowsRenderer.h"
#include "shadow/BumpMappedShadowsRenderer.h"
#include "shadow/DefaultShadowsRenderer.h"
#include "shadow/TerrainBumplMappedShadowsRenderer.h"
#include "shadow/TerrainShadowsRenderer.h"
#include "sky/LensFlareRenderer.h"
#include "sky/SkyBoxRenderer.h"
#include "sky/SkyRenderer.h"
#include "sky/SunRenderer.h"
#include "terrain/TerrainConeStepMappedRenderer.h"
#include "terrain/TerrainNormalMappedRenderer.h"
#include "terrain/TerrainParallaxMappedRenderer.h"
#include "terrain/TerrainRenderer.h"
#include "water/WaterRenderer.h"

#include <prev/core/DeviceProvider.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer {
MasterRenderer::MasterRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::Swapchain>& swapchain)
    : m_defaultRenderPass(renderPass)
    , m_swapchain(swapchain)
{
}

void MasterRenderer::Init()
{
    InitDefault();
    InitDebug();
    InitShadows();
    InitReflection();
    InitRefraction();
}

void MasterRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void MasterRenderer::PreRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void MasterRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
    // Shadows render pass
    RenderShadows(renderContext, node);

    // Reflection
    RenderSceneReflection(renderContext, node);

    // Refraction
    RenderSceneRefraction(renderContext, node);

    // Default Scene Render
    RenderScene(renderContext, node);

#ifndef ANDROID
    // Debug quad with shadowMap
    //RenderDebug(renderContext, node);
#endif
}

void MasterRenderer::PostRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void MasterRenderer::AfterRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void MasterRenderer::ShutDown()
{
    ShutDownRefraction();
    ShutDownReflection();
    ShutDownShadows();
    ShutDownDebug();
    ShutDownDefault();
}

void MasterRenderer::InitDefault()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyBoxRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::DefaultRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::TexturelessRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::NormalMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ParallaxMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ConeStepMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainNormalMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainParallaxMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainConeStepMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationTexturelessRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationNormalMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationParallaxMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationConeStepMappedRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::water::WaterRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::font::FontRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::particle::ParticlesRenderer>(m_defaultRenderPass));
#ifdef RENDER_BOUNDING_VOLUMES
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::BoundingVolumeDebugRenderer>(m_defaultRenderPass));
#endif
#ifdef RENDER_RAYCASTS
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::RayCastDebugRenderer>(m_defaultRenderPass));
#endif
#ifdef RENDER_SELECTION
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::SelectionDebugRenderer>(m_defaultRenderPass));
#endif
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SunRenderer>(m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::LensFlareRenderer>(m_defaultRenderPass));

    for (auto& renderer : m_defaultRenderers) {
        renderer->Init();
    }

#ifdef PARALLEL_RENDERING
    CommandBuffersGroupFactory buffersGroupFactory{};
    m_defaultCommandBuffersGroup = buffersGroupFactory.CreateGroup(*device, *device->GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchain->GetImageCount(), static_cast<uint32_t>(m_defaultRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
#endif
}

void MasterRenderer::ShutDownDefault()
{
#ifdef PARALLEL_RENDERING
    m_defaultCommandBuffersGroup = nullptr;
#endif

    for (auto it = m_defaultRenderers.rbegin(); it != m_defaultRenderers.rend(); ++it) {
        (*it)->ShutDown();
    }
}

void MasterRenderer::InitDebug()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    m_debugRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::ShadowMapDebugRenderer>(m_defaultRenderPass));
    m_debugRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::TextureDebugRenderer>(m_defaultRenderPass));

    for (auto& renderer : m_debugRenderers) {
        renderer->Init();
    }

#ifdef PARALLEL_RENDERING
    CommandBuffersGroupFactory buffersGroupFactory{};
    m_debugCommandBuffersGroup = buffersGroupFactory.CreateGroup(*device, *device->GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchain->GetImageCount(), static_cast<uint32_t>(m_debugRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
#endif
}

void MasterRenderer::ShutDownDebug()
{
#ifdef PARALLEL_RENDERING
    m_debugCommandBuffersGroup = nullptr;
#endif

    for (auto it = m_debugRenderers.rbegin(); it != m_debugRenderers.rend(); ++it) {
        (*it)->ShutDown();
    }
}

void MasterRenderer::InitShadows()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    const auto shadowsComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW }) };

    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::DefaultShadowsRenderer>(shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::BumpMappedShadowsRenderer>(shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::TerrainShadowsRenderer>(shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::TerrainBumplMappedShadowsRenderer>(shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::AnimationShadowsRenderer>(shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::AnimationBumpMappedShadowsRenderer>(shadowsComponent->GetRenderPass()));

    for (auto& shadowRenderer : m_shadowRenderers) {
        shadowRenderer->Init();
    }

#ifdef PARALLEL_RENDERING
    CommandBuffersGroupFactory buffersGroupFactory{};
    for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; i++) {
        m_shadowsCommandBufferGroups.emplace_back(buffersGroupFactory.CreateGroup(*device, *device->GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchain->GetImageCount(), static_cast<uint32_t>(m_shadowRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY));
    }
#endif
}

void MasterRenderer::ShutDownShadows()
{
#ifdef PARALLEL_RENDERING
    m_shadowsCommandBufferGroups.clear();
#endif

    for (auto it = m_shadowRenderers.rbegin(); it != m_shadowRenderers.rend(); ++it) {
        (*it)->ShutDown();
    }
}

void MasterRenderer::InitReflection()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    const auto reflectionComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFLECTION_RENDER_COMPONENT }) };

    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyBoxRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::DefaultRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::TexturelessRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::NormalMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ParallaxMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ConeStepMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainNormalMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainParallaxMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainConeStepMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationTexturelessRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationNormalMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationParallaxMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationConeStepMappedRenderer>(reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::particle::ParticlesRenderer>(reflectionComponent->GetRenderPass()));

    for (auto& shadowRenderer : m_reflectionRenderers) {
        shadowRenderer->Init();
    }

#ifdef PARALLEL_RENDERING
    CommandBuffersGroupFactory buffersGroupFactory{};
    m_reflectionCommandBufferGroups = buffersGroupFactory.CreateGroup(*device, *device->GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchain->GetImageCount(), static_cast<uint32_t>(m_reflectionRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
#endif
}

void MasterRenderer::ShutDownReflection()
{
#ifdef PARALLEL_RENDERING
    m_reflectionCommandBufferGroups = nullptr;
#endif

    for (auto it = m_reflectionRenderers.rbegin(); it != m_reflectionRenderers.rend(); ++it) {
        (*it)->ShutDown();
    }
}

void MasterRenderer::InitRefraction()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    const auto refractionComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFRACTION_RENDER_COMPONENT }) };

    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyBoxRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::DefaultRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::TexturelessRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::NormalMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ParallaxMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ConeStepMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainNormalMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainParallaxMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainConeStepMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationTexturelessRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationNormalMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationParallaxMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationConeStepMappedRenderer>(refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::particle::ParticlesRenderer>(refractionComponent->GetRenderPass()));

    for (auto& shadowRenderer : m_refractionRenderers) {
        shadowRenderer->Init();
    }

#ifdef PARALLEL_RENDERING
    CommandBuffersGroupFactory buffersGroupFactory{};
    m_refractionCommandBufferGroups = buffersGroupFactory.CreateGroup(*device, *device->GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchain->GetImageCount(), static_cast<uint32_t>(m_refractionRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
#endif
}

void MasterRenderer::ShutDownRefraction()
{
#ifdef PARALLEL_RENDERING
    m_refractionCommandBufferGroups = nullptr;
#endif

    for (auto it = m_refractionRenderers.rbegin(); it != m_refractionRenderers.rend(); ++it) {
        (*it)->ShutDown();
    }
}

void MasterRenderer::RenderShadows(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const auto shadows{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW }) };

    for (uint32_t cascadeIndex = 0; cascadeIndex < prev_test::component::shadow::CASCADES_COUNT; cascadeIndex++) {

        const auto& cascade{ shadows->GetCascade(cascadeIndex) };

        const ShadowsRenderContextUserData userData{ cascade.viewMatrix, cascade.projectionMatrix, cascadeIndex, prev_test::common::intersection::Frustum{ cascade.projectionMatrix, cascade.viewMatrix } };
        const prev::render::RenderContext customRenderContext{ cascade.frameBuffer, renderContext.commandBuffer, renderContext.frameInFlightIndex, { { 0, 0 }, shadows->GetExtent() } };

        for (auto& renderer : m_shadowRenderers) {
            renderer->BeforeRender(customRenderContext, userData);
        }

#ifdef PARALLEL_RENDERING
        const auto& cascadeCommandBuffers{ m_shadowsCommandBufferGroups.at(cascadeIndex)->GetBuffersGroup(renderContext.frameInFlightIndex) };
        RenderParallel(shadows->GetRenderPass(), customRenderContext, root, m_shadowRenderers, cascadeCommandBuffers, userData);
#else
        RenderSerial(shadows->GetRenderPass(), customRenderContext, root, m_shadowRenderers, userData);
#endif
        for (auto& renderer : m_shadowRenderers) {
            renderer->AfterRender(customRenderContext, userData);
        }
    }
}

void MasterRenderer::RenderSceneReflection(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const auto reflectionComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFLECTION_RENDER_COMPONENT }) };
    const auto cameraComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA }) };

    const auto& cameraPosition{ cameraComponent->GetPosition() };
    const auto cameraViewPosition{ cameraComponent->GetPosition() + cameraComponent->GetForwardDirection() };
    const float cameraPositionOffset{ 2.0f * (cameraPosition.y - prev_test::component::water::WATER_LEVEL) };
    const float cameraViewOffset{ 2.0f * (prev_test::component::water::WATER_LEVEL - cameraViewPosition.y) };

    const glm::vec3 newCameraPosition{ cameraPosition.x, cameraPosition.y - cameraPositionOffset, cameraPosition.z };
    const glm::vec3 newCameraViewPosition{ cameraViewPosition.x, cameraViewPosition.y + cameraViewOffset, cameraViewPosition.z };
    const glm::mat4 viewMatrix = glm::lookAt(newCameraPosition, newCameraViewPosition, cameraComponent->GetUpDirection());
    const glm::mat4 projectionMatrix = cameraComponent->GetViewFrustum().CreateProjectionMatrix(reflectionComponent->GetExtent().width, reflectionComponent->GetExtent().height);

    const NormalRenderContextUserData userData{
        viewMatrix,
        projectionMatrix,
        newCameraPosition,
        glm::vec4(0.0f, 1.0f, 0.0f, -(prev_test::component::water::WATER_LEVEL + prev_test::component::water::WATER_CLIP_PLANE_OFFSET)),
        glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane()),
        prev_test::common::intersection::Frustum{ projectionMatrix, viewMatrix }
    };

    const prev::render::RenderContext customRenderContext{ reflectionComponent->GetFrameBuffer(), renderContext.commandBuffer, renderContext.frameInFlightIndex, { { 0, 0 }, reflectionComponent->GetExtent() } };

    for (auto& renderer : m_reflectionRenderers) {
        renderer->BeforeRender(customRenderContext, userData);
    }

#ifdef PARALLEL_RENDERING
    const auto& commandBuffers{ m_reflectionCommandBufferGroups->GetBuffersGroup(renderContext.frameInFlightIndex) };
    RenderParallel(reflectionComponent->GetRenderPass(), customRenderContext, root, m_reflectionRenderers, commandBuffers, userData);
#else
    RenderSerial(reflectionComponent->GetRenderPass(), customRenderContext, root, m_reflectionRenderers, userData);
#endif

    for (auto& renderer : m_reflectionRenderers) {
        renderer->AfterRender(customRenderContext, userData);
    }
}

void MasterRenderer::RenderSceneRefraction(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const auto refractionComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFRACTION_RENDER_COMPONENT }) };
    const auto cameraComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA }) };

    const auto& viewMatrix{ cameraComponent->LookAt() };
    const auto projectionMatrix{ cameraComponent->GetViewFrustum().CreateProjectionMatrix(refractionComponent->GetExtent().width, refractionComponent->GetExtent().height) };
    const NormalRenderContextUserData userData{
        viewMatrix,
        projectionMatrix,
        cameraComponent->GetPosition(),
        glm::vec4(0.0f, -1.0f, 0.0f, prev_test::component::water::WATER_LEVEL + prev_test::component::water::WATER_CLIP_PLANE_OFFSET),
        glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane()),
        prev_test::common::intersection::Frustum{ projectionMatrix, viewMatrix }
    };

    const prev::render::RenderContext customRenderContext{ refractionComponent->GetFrameBuffer(), renderContext.commandBuffer, renderContext.frameInFlightIndex, { { 0, 0 }, refractionComponent->GetExtent() } };

    for (auto& renderer : m_refractionRenderers) {
        renderer->BeforeRender(customRenderContext, userData);
    }

#ifdef PARALLEL_RENDERING
    const auto& commandBuffers{ m_refractionCommandBufferGroups->GetBuffersGroup(renderContext.frameInFlightIndex) };
    RenderParallel(refractionComponent->GetRenderPass(), customRenderContext, root, m_refractionRenderers, commandBuffers, userData);
#else
    RenderSerial(refractionComponent->GetRenderPass(), customRenderContext, root, m_refractionRenderers, userData);
#endif

    for (auto& renderer : m_refractionRenderers) {
        renderer->AfterRender(customRenderContext, userData);
    }
}

void MasterRenderer::RenderScene(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const auto cameraComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA }) };

    const auto& viewMatrix{ cameraComponent->LookAt() };
    const auto projectionMatrix{ cameraComponent->GetViewFrustum().CreateProjectionMatrix(renderContext.rect.extent.width, renderContext.rect.extent.height) };
    const NormalRenderContextUserData userData{
        viewMatrix,
        projectionMatrix,
        cameraComponent->GetPosition(),
        DEFAULT_CLIP_PLANE,
        glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane()),
        prev_test::common::intersection::Frustum{ projectionMatrix, viewMatrix }
    };

    for (auto& renderer : m_defaultRenderers) {
        renderer->BeforeRender(renderContext, userData);
    }

#ifdef PARALLEL_RENDERING
    const auto& commandBuffers{ m_defaultCommandBuffersGroup->GetBuffersGroup(renderContext.frameInFlightIndex) };
    RenderParallel(m_defaultRenderPass, renderContext, root, m_defaultRenderers, commandBuffers, userData);
#else
    RenderSerial(m_defaultRenderPass, renderContext, root, m_defaultRenderers, userData);
#endif

    for (auto& renderer : m_defaultRenderers) {
        renderer->AfterRender(renderContext, userData);
    }
}

void MasterRenderer::RenderDebug(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    for (auto& renderer : m_debugRenderers) {
        renderer->BeforeRender(renderContext);
    }

    const prev::render::RenderContext customRenderContext{ renderContext.frameBuffer, renderContext.commandBuffer, renderContext.frameInFlightIndex, { { 0, 0 }, { renderContext.rect.extent.width / 2, renderContext.rect.extent.height / 2 } } };

#ifdef PARALLEL_RENDERING
    const auto& debugCommandBuffers{ m_debugCommandBuffersGroup->GetBuffersGroup(renderContext.frameInFlightIndex) };
    RenderParallel(m_defaultRenderPass, customRenderContext, root, m_debugRenderers, debugCommandBuffers, {});
#else
    RenderSerial(m_defaultRenderPass, customRenderContext, root, m_debugRenderers, {});
#endif

    for (auto& renderer : m_debugRenderers) {
        renderer->AfterRender(renderContext);
    }
}
} // namespace prev_test::render::renderer