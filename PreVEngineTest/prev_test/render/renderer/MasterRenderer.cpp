#include "MasterRenderer.h"
#include "CommandBuffersGroupFactory.h"
#include "animation/AnimationConeStepMappedRenderer.h"
#include "animation/AnimationNormalMappedRenderer.h"
#include "animation/AnimationRenderer.h"
#include "animation/AnimationTexturelessRenderer.h"
#include "debug/BoundingVolumeDebugRenderer.h"
#include "debug/RayCastDebugRenderer.h"
#include "debug/SelectionDebugRenderer.h"
#include "debug/ShadowMapDebugRenderer.h"
#include "debug/TextureDebugRenderer.h"
#include "font/Font3dRenderer.h"
#include "font/FontRenderer.h"
#include "normal/ConeStepMappedRenderer.h"
#include "normal/DefaultRenderer.h"
#include "normal/NormalMappedRenderer.h"
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
#include "terrain/TerrainRenderer.h"
#include "water/WaterRenderer.h"

#include "../../common/intersection/Frustum.h"
#include "../../component/camera/ICameraComponent.h"
#include "../../component/common/IOffScreenRenderPassComponent.h"
#include "../../component/shadow/IShadowsComponent.h"
#include "../../component/water/IWaterComponent.h"
#include "../../component/water/WaterCommon.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer {
MasterRenderer::MasterRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, uint32_t swapchainImageCount, uint32_t viewCount)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_defaultRenderPass{ renderPass }
    , m_swapchainImageCount{ swapchainImageCount }
    , m_viewCount{ viewCount }
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

void MasterRenderer::Render(const prev::render::RenderContext& renderContext, const prev::scene::IScene& scene)
{
    // Shadows render pass
    RenderShadows(renderContext, scene.GetRootNode());

    // Reflection
    RenderSceneReflection(renderContext, scene.GetRootNode());

    // Refraction
    RenderSceneRefraction(renderContext, scene.GetRootNode());

    // Default Scene Render
    RenderScene(renderContext, scene.GetRootNode());

#ifndef ANDROID
    // Debug quad with shadowMap
    // RenderDebug(renderContext, scene->GetRootNode());
#endif
}

void MasterRenderer::ShutDown()
{
    ShutDownRefraction();
    ShutDownReflection();
    ShutDownShadows();
    ShutDownDebug();
    ShutDownDefault();
}

void MasterRenderer::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_R) {
            m_device.WaitIdle();

            ShutDown();
            Init();
        }
    }
}

void MasterRenderer::InitDefault()
{
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyBoxRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::DefaultRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::TexturelessRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::NormalMappedRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ConeStepMappedRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainNormalMappedRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainConeStepMappedRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationTexturelessRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationNormalMappedRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationConeStepMappedRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::water::WaterRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::font::FontRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::font::Font3dRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::particle::ParticlesRenderer>(m_device, m_allocator, m_defaultRenderPass));
#ifdef RENDER_BOUNDING_VOLUMES
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::BoundingVolumeDebugRenderer>(m_device, m_allocator, m_defaultRenderPass));
#endif
#ifdef RENDER_RAYCASTS
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::RayCastDebugRenderer>(m_device, m_allocator, m_defaultRenderPass));
#endif
#ifdef RENDER_SELECTION
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::SelectionDebugRenderer>(m_device, m_allocator, m_defaultRenderPass));
#endif
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SunRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_defaultRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::LensFlareRenderer>(m_device, m_allocator, m_defaultRenderPass));

    for (auto& renderer : m_defaultRenderers) {
        renderer->Init();
    }

#ifdef PARALLEL_RENDERING
    m_defaultCommandBuffersGroup = CommandBuffersGroupFactory{}.CreateGroup(m_device, *m_device.GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchainImageCount, static_cast<uint32_t>(m_defaultRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
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

    m_defaultRenderers.clear();
}

void MasterRenderer::InitDebug()
{
    // m_debugRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::ShadowMapDebugRenderer>(m_device, m_allocator, m_defaultRenderPass));
    m_debugRenderers.emplace_back(std::make_unique<prev_test::render::renderer::debug::TextureDebugRenderer>(m_device, m_allocator, m_defaultRenderPass));

    for (auto& renderer : m_debugRenderers) {
        renderer->Init();
    }

#ifdef PARALLEL_RENDERING
    m_debugCommandBuffersGroup = CommandBuffersGroupFactory{}.CreateGroup(m_device, *m_device.GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchainImageCount, static_cast<uint32_t>(m_debugRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
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

    m_debugRenderers.clear();
}

void MasterRenderer::InitShadows()
{
    const auto shadowsComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW }) };

    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::DefaultShadowsRenderer>(m_device, m_allocator, *shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::BumpMappedShadowsRenderer>(m_device, m_allocator, *shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::TerrainShadowsRenderer>(m_device, m_allocator, *shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::TerrainBumplMappedShadowsRenderer>(m_device, m_allocator, *shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::AnimationShadowsRenderer>(m_device, m_allocator, *shadowsComponent->GetRenderPass()));
    m_shadowRenderers.emplace_back(std::make_unique<prev_test::render::renderer::shadow::AnimationBumpMappedShadowsRenderer>(m_device, m_allocator, *shadowsComponent->GetRenderPass()));

    for (auto& renderer : m_shadowRenderers) {
        renderer->Init();
    }

#ifdef PARALLEL_RENDERING
    m_shadowsCommandBufferGroups.resize(prev_test::component::shadow::CASCADES_COUNT);
    for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; ++i) {
        m_shadowsCommandBufferGroups[i] = CommandBuffersGroupFactory{}.CreateGroup(m_device, *m_device.GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchainImageCount, static_cast<uint32_t>(m_shadowRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
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

    m_shadowRenderers.clear();
}

void MasterRenderer::InitReflection()
{
    const auto reflectionComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFLECTION_RENDER_COMPONENT }) };

    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyBoxRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::DefaultRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::TexturelessRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::NormalMappedRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ConeStepMappedRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainNormalMappedRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainConeStepMappedRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationTexturelessRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationNormalMappedRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationConeStepMappedRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));
    m_reflectionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::particle::ParticlesRenderer>(m_device, m_allocator, *reflectionComponent->GetRenderPass()));

    for (auto& renderer : m_reflectionRenderers) {
        renderer->Init();
    }

#ifdef PARALLEL_RENDERING
    m_reflectionCommandBufferGroups = CommandBuffersGroupFactory{}.CreateGroup(m_device, *m_device.GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchainImageCount, static_cast<uint32_t>(m_reflectionRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
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

    m_reflectionRenderers.clear();
}

void MasterRenderer::InitRefraction()
{
    const auto refractionComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFRACTION_RENDER_COMPONENT }) };

    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyBoxRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::sky::SkyRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::DefaultRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::TexturelessRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::NormalMappedRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::normal::ConeStepMappedRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainNormalMappedRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::terrain::TerrainConeStepMappedRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationTexturelessRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationNormalMappedRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::animation::AnimationConeStepMappedRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));
    m_refractionRenderers.emplace_back(std::make_unique<prev_test::render::renderer::particle::ParticlesRenderer>(m_device, m_allocator, *refractionComponent->GetRenderPass()));

    for (auto& renderer : m_refractionRenderers) {
        renderer->Init();
    }

#ifdef PARALLEL_RENDERING
    m_refractionCommandBufferGroups = CommandBuffersGroupFactory{}.CreateGroup(m_device, *m_device.GetQueue(prev::core::device::QueueType::GRAPHICS), m_swapchainImageCount, static_cast<uint32_t>(m_refractionRenderers.size()), VK_COMMAND_BUFFER_LEVEL_SECONDARY);
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

    m_refractionRenderers.clear();
}

void MasterRenderer::RenderShadows(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const auto shadows{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW }) };

    for (uint32_t cascadeIndex = 0; cascadeIndex < prev_test::component::shadow::CASCADES_COUNT; ++cascadeIndex) {

        const auto& cascade{ shadows->GetCascade(cascadeIndex) };

        const prev::render::RenderContext customRenderContextBase{ cascade.frameBuffer, renderContext.commandBuffer, renderContext.frameInFlightIndex, { { 0, 0 }, shadows->GetExtent() } };
        const ShadowsRenderContext customRenderContext{ customRenderContextBase, cascade.viewMatrix, cascade.projectionMatrix, cascadeIndex, prev_test::common::intersection::Frustum{ cascade.projectionMatrix, cascade.viewMatrix } };

#ifdef PARALLEL_RENDERING
        const auto& cascadeCommandBuffers{ m_shadowsCommandBufferGroups[cascadeIndex]->GetBuffersGroup(customRenderContext.frameInFlightIndex) };
        RenderParallel(*shadows->GetRenderPass(), customRenderContext, root, m_shadowRenderers, cascadeCommandBuffers);
#else
        RenderSerial(*shadows->GetRenderPass(), customRenderContext, root, m_shadowRenderers);
#endif
    }
}

void MasterRenderer::RenderSceneReflection(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const auto reflectionComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFLECTION_RENDER_COMPONENT }) };
    const auto cameraComponents{ prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA }) };

    const prev::render::RenderContext customRenderContextBase{ reflectionComponent->GetFrameBuffer(), renderContext.commandBuffer, renderContext.frameInFlightIndex,
        { { 0, 0 }, reflectionComponent->GetExtent() } };
    // TODO -> refactor this - shall we create the customRenderContext in one step - using constructor ??
    NormalRenderContext customRenderContext{
        customRenderContextBase,
        glm::vec4(0.0f, 1.0f, 0.0f, -(prev_test::component::water::WATER_LEVEL + prev_test::component::water::WATER_CLIP_PLANE_OFFSET)),
        static_cast<uint32_t>(cameraComponents.size())
    };

    for (uint32_t view = 0; view < static_cast<uint32_t>(cameraComponents.size()); ++view) {
        const auto& cameraComponent{ cameraComponents[view] };

        const auto& cameraPosition{ cameraComponent->GetPosition() };
        const auto cameraViewPosition{ cameraComponent->GetPosition() + cameraComponent->GetForwardDirection() };

        const float cameraPositionOffset{ 2.0f * (cameraPosition.y - prev_test::component::water::WATER_LEVEL) };
        const float cameraViewOffset{ 2.0f * (cameraViewPosition.y - prev_test::component::water::WATER_LEVEL) };

        const glm::vec3 newCameraPosition{ cameraPosition.x, cameraPosition.y - cameraPositionOffset, cameraPosition.z };
        const glm::vec3 newCameraViewPosition{ cameraViewPosition.x, cameraViewPosition.y - cameraViewOffset, cameraViewPosition.z };

        const auto reflectedUpDirection{ glm::reflect(-cameraComponent->GetUpDirection(), cameraComponent->GetDefaultUpDirection()) };

        const glm::mat4 viewMatrix{ glm::lookAt(newCameraPosition, newCameraViewPosition, reflectedUpDirection) };
        const glm::mat4 projectionMatrix{ cameraComponent->GetViewFrustum().CreateProjectionMatrix() };

        customRenderContext.nearFarClippingPlanes[view] = glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane());
        customRenderContext.viewMatrices[view] = viewMatrix;
        customRenderContext.projectionMatrices[view] = projectionMatrix;
        customRenderContext.cameraPositions[view] = newCameraPosition;
        customRenderContext.frustums[view] = prev_test::common::intersection::Frustum{ projectionMatrix, viewMatrix };
    }

#ifdef PARALLEL_RENDERING
    const auto& commandBuffers{ m_reflectionCommandBufferGroups->GetBuffersGroup(customRenderContext.frameInFlightIndex) };
    RenderParallel(*reflectionComponent->GetRenderPass(), customRenderContext, root, m_reflectionRenderers, commandBuffers);
#else
    RenderSerial(*reflectionComponent->GetRenderPass(), customRenderContext, root, m_reflectionRenderers);
#endif
}

void MasterRenderer::RenderSceneRefraction(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const auto refractionComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFRACTION_RENDER_COMPONENT }) };
    const auto cameraComponents{ prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA }) };

    const prev::render::RenderContext customRenderContextBase{ refractionComponent->GetFrameBuffer(), renderContext.commandBuffer, renderContext.frameInFlightIndex, { { 0, 0 }, refractionComponent->GetExtent() } };

    // TODO -> refactor this - shall we create the customRenderContext in one step - using constructor ??
    NormalRenderContext customRenderContext{
        customRenderContextBase,
        glm::vec4(0.0f, -1.0f, 0.0f, prev_test::component::water::WATER_LEVEL + prev_test::component::water::WATER_CLIP_PLANE_OFFSET),
        static_cast<uint32_t>(cameraComponents.size())
    };

    for (uint32_t view = 0; view < static_cast<uint32_t>(cameraComponents.size()); ++view) {
        const auto& cameraComponent{ cameraComponents[view] };

        const auto& viewMatrix{ cameraComponent->LookAt() };
        const auto projectionMatrix{ cameraComponent->GetViewFrustum().CreateProjectionMatrix() };

        customRenderContext.nearFarClippingPlanes[view] = glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane());
        customRenderContext.viewMatrices[view] = viewMatrix;
        customRenderContext.projectionMatrices[view] = projectionMatrix;
        customRenderContext.cameraPositions[view] = cameraComponent->GetPosition();
        customRenderContext.frustums[view] = prev_test::common::intersection::Frustum{ projectionMatrix, viewMatrix };
    }

#ifdef PARALLEL_RENDERING
    const auto& commandBuffers{ m_refractionCommandBufferGroups->GetBuffersGroup(customRenderContext.frameInFlightIndex) };
    RenderParallel(*refractionComponent->GetRenderPass(), customRenderContext, root, m_refractionRenderers, commandBuffers);
#else
    RenderSerial(*refractionComponent->GetRenderPass(), customRenderContext, root, m_refractionRenderers);
#endif
}

void MasterRenderer::RenderScene(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const auto cameraComponents{ prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA }) };

    // TODO -> refactor this - shall we create the customRenderContext in one step - using constructor ??
    NormalRenderContext customRenderContext{
        renderContext,
        DEFAULT_CLIP_PLANE,
        static_cast<uint32_t>(cameraComponents.size())
    };

    for (uint32_t view = 0; view < static_cast<uint32_t>(cameraComponents.size()); ++view) {
        const auto& cameraComponent{ cameraComponents[view] };

        const auto& viewMatrix{ cameraComponent->LookAt() };
        const auto projectionMatrix{ cameraComponent->GetViewFrustum().CreateProjectionMatrix() };

        customRenderContext.nearFarClippingPlanes[view] = glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane());
        customRenderContext.viewMatrices[view] = viewMatrix;
        customRenderContext.projectionMatrices[view] = projectionMatrix;
        customRenderContext.cameraPositions[view] = cameraComponent->GetPosition();
        customRenderContext.frustums[view] = prev_test::common::intersection::Frustum{ projectionMatrix, viewMatrix };
    }

#ifdef PARALLEL_RENDERING
    cameraComponentsconst auto& commandBuffers{ m_defaultCommandBuffersGroup->GetBuffersGroup(customRenderContext.frameInFlightIndex) };
    RenderParallel(m_defaultRenderPass, customRenderContext, root, m_defaultRenderers, commandBuffers);
#else
    RenderSerial(m_defaultRenderPass, customRenderContext, root, m_defaultRenderers);
#endif
}

void MasterRenderer::RenderDebug(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root)
{
    const prev::render::RenderContext customRenderContext{ renderContext.frameBuffer, renderContext.commandBuffer, renderContext.frameInFlightIndex, { { 0, 0 }, { renderContext.rect.extent.width / 2, renderContext.rect.extent.height / 2 } } };

#ifdef PARALLEL_RENDERING
    const auto& debugCommandBuffers{ m_debugCommandBuffersGroup->GetBuffersGroup(customRenderContext.frameInFlightIndex) };
    RenderParallel(m_defaultRenderPass, customRenderContext, root, m_debugRenderers, debugCommandBuffers);
#else
    RenderSerial(m_defaultRenderPass, customRenderContext, root, m_debugRenderers);
#endif
}
} // namespace prev_test::render::renderer