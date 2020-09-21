#ifndef __SUN_RENDERER_H__
#define __SUN_RENDERER_H__

#include "../../General.h"
#include "../IRenderer.h"
#include "../pipeline/IPipeline.h"

#include <render/pass/RenderPass.h>
#include <render/shader/Shader.h>
#include <scene/graph/ISceneNode.h>

#include <memory>

namespace prev_test::render::sky {
class SunRenderer final : public prev_test::render::IRenderer<NormalRenderContextUserData> {
public:
    SunRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~SunRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    struct alignas(16) UniformsVS
    {
        alignas(16) glm::vec4 translation;

        alignas(16) glm::vec4 scale;
    };

private:
    const uint32_t m_descriptorCount{ 50 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsVS> > m_uniformsPoolVS;

private:
    uint64_t m_passedSamples{ 0 };

    uint64_t m_maxNumberOfSamples{ 0 };

    VkQueryPool m_queryPool{ nullptr };
};
} // namespace prev_test::render::sky

#endif // !__SUN_RENDERER_H__
