#ifndef __TEXTURE_DEBUG_RENDERER_H__
#define __TEXTURE_DEBUG_RENDERER_H__

#include "../../IModel.h"
#include "../IRenderer.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::debug {
class TextureDebugRenderer final : public IRenderer<prev::render::RenderContext> {
public:
    TextureDebugRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~TextureDebugRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext) override;

    void PreRender(const prev::render::RenderContext& renderContext) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const prev::render::RenderContext& renderContext) override;

    void AfterRender(const prev::render::RenderContext& renderContext) override;

    void ShutDown() override;

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_renderPass;

    prev::scene::IScene& m_scene;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_pipeline;

    std::unique_ptr<prev::render::sampler::Sampler> m_colorSampler;

private:
    std::unique_ptr<prev_test::render::IModel> m_quadModel;
};
} // namespace prev_test::render::renderer::debug

#endif // !__TEXTURE_DEBUG_RENDERER_H__
