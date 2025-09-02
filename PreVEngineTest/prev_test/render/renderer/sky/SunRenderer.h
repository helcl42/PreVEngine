#ifndef __SUN_RENDERER_H__
#define __SUN_RENDERER_H__

#include "../IRenderer.h"
#include "../RenderContexts.h"

#include <prev/render/buffer/BufferPool.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/query/QueryPool.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>
#include <prev/util/Utils.h>

namespace prev_test::render::renderer::sky {
class SunRenderer final : public IRenderer<NormalRenderContext> {
public:
    SunRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~SunRenderer() = default;

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
        DEFAULT_ALIGNMENT glm::vec4 translations[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT glm::vec4 scale;
    };

private:
    const uint32_t m_descriptorCount{ 50 };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_renderPass;

    prev::scene::IScene& m_scene;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::BufferPool> m_uniformsPoolVS;

private:
    uint64_t m_passedSamples{ 0 };

    uint64_t m_maxNumberOfSamples{ 0 };

    static const inline uint32_t QueryPoolCount{ 3 };

    prev::util::CircularIndex<uint32_t> m_queryPoolIndex{ QueryPoolCount };

    std::unique_ptr<prev::render::query::QueryPool> m_queryPool{};
};
} // namespace prev_test::render::renderer::sky

#endif // !__SUN_RENDERER_H__
