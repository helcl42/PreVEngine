#ifndef __ANIMATION_SHADOWS_RENDERER_H__
#define __ANIMATION_SHADOWS_RENDERER_H__

#include "../../IAnimation.h"
#include "../../IMesh.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::shadow {
class AnimationShadowsRenderer final : public IRenderer<ShadowsRenderContext> {
public:
    AnimationShadowsRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass);

    ~AnimationShadowsRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const ShadowsRenderContext& renderContext) override;

    void PreRender(const ShadowsRenderContext& renderContext) override;

    void Render(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const ShadowsRenderContext& renderContext) override;

    void AfterRender(const ShadowsRenderContext& renderContext) override;

    void ShutDown() override;

private:
    void RenderMeshNode(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev_test::render::MeshNode& meshNode);

private:
    struct Uniforms {
        DEFAULT_ALIGNMENT glm::mat4 bones[MAX_BONES_COUNT];
        DEFAULT_ALIGNMENT glm::mat4 modelMatrix;
        DEFAULT_ALIGNMENT glm::mat4 viewMatrix;
        DEFAULT_ALIGNMENT glm::mat4 projectionMatrix;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UniformBufferRing<Uniforms>> m_uniformsPool;
};
} // namespace prev_test::render::renderer::shadow

#endif // !__ANIMATION_SHADOWS_RENDERER_H__
