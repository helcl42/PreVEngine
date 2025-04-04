#ifndef __FONT_RENDERER_H__
#define __FONT_RENDERER_H__

#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::font {
class FontRenderer final : public IRenderer<NormalRenderContext> {
public:
    FontRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~FontRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const NormalRenderContext& renderContext) override;

    void PreRender(const NormalRenderContext& renderContext) override;

    void Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const NormalRenderContext& renderContext) override;

    void AfterRender(const NormalRenderContext& renderContext) override;

    void ShutDown() override;

private:
    struct DEFAULT_ALIGNMENT UniformsVS {
        DEFAULT_ALIGNMENT glm::vec4 translation;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT glm::vec4 color;

        DEFAULT_ALIGNMENT glm::vec4 width;

        DEFAULT_ALIGNMENT glm::vec4 edge;

        DEFAULT_ALIGNMENT glm::vec4 bias;

        DEFAULT_ALIGNMENT uint32_t hasEffect;

        DEFAULT_ALIGNMENT glm::vec4 borderWidth;

        DEFAULT_ALIGNMENT glm::vec4 borderEdge;

        DEFAULT_ALIGNMENT glm::vec4 outlineColor;

        DEFAULT_ALIGNMENT glm::vec4 outlineOffset;
    };

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

    std::unique_ptr<prev::render::buffer::UniformRingBuffer<UniformsVS>> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::UniformRingBuffer<UniformsFS>> m_uniformsPoolFS;
};
} // namespace prev_test::render::renderer::font

#endif // !__FONT_RENDERER_H__
