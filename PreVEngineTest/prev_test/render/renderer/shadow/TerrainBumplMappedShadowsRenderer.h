#ifndef __TERRAIN_BUMP_MAPPED_SHADOWS_RENDERER_H__
#define __TERRAIN_BUMP_MAPPED_SHADOWS_RENDERER_H__

#include "../IRenderer.h"
#include "../RenderContexts.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/BufferPool.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::shadow {
class TerrainBumplMappedShadowsRenderer final : public IRenderer<ShadowsRenderContext> {
public:
    TerrainBumplMappedShadowsRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~TerrainBumplMappedShadowsRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const ShadowsRenderContext& renderContext) override;

    void PreRender(const ShadowsRenderContext& renderContext) override;

    void Render(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const ShadowsRenderContext& renderContext) override;

    void AfterRender(const ShadowsRenderContext& renderContext) override;

    void ShutDown() override;

private:
    struct Uniforms {
        DEFAULT_ALIGNMENT glm::mat4 modelMatrix;
        DEFAULT_ALIGNMENT glm::mat4 viewMatrix;
        DEFAULT_ALIGNMENT glm::mat4 projectionMatrix;
    };

private:
    const uint32_t m_descriptorCount{ 3000 };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_renderPass;

    prev::scene::IScene& m_scene;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::BufferPool> m_uniformsPool;
};
} // namespace prev_test::render::renderer::shadow

#endif // !__TERRAIN_BUMP_MAPPED_SHADOWS_RENDERER_H__
