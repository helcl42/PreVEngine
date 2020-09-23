#ifndef __ANIMATION_RENDERER_H__
#define __ANIMATION_RENDERER_H__

#include "../../../General.h"
#include "../../../Shadows.h"
#include "../IRenderer.h"
#include "../../pipeline/IPipeline.h"

#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

#include <memory>

namespace prev_test::render::renderer::animation {
class AnimationRenderer final : public IRenderer<NormalRenderContextUserData> {
public:
    AnimationRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~AnimationRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;
    
    void PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    struct ShadowsCascadeUniform {
        glm::mat4 viewProjectionMatrix;

        glm::vec4 split;

        ShadowsCascadeUniform() = default;

        ShadowsCascadeUniform(const glm::mat4& vpMat, const glm::vec4& spl)
            : viewProjectionMatrix(vpMat)
            , split(spl)
        {
        }
    };

    struct ShadowsUniform {
        ShadowsCascadeUniform cascades[ShadowsComponent::CASCADES_COUNT];

        uint32_t enabled;
    };

    struct LightUniform {
        glm::vec4 position;

        glm::vec4 color;

        glm::vec4 attenuation;

        LightUniform() = default;

        LightUniform(const glm::vec4& pos, const glm::vec4& col, const glm::vec4& atten)
            : position(pos)
            , color(col)
            , attenuation(atten)
        {
        }
    };

    struct LightningUniform {
        LightUniform lights[MAX_LIGHT_COUNT];

        uint32_t realCountOfLights;

        float ambientFactor;
    };

    struct MaterialUniform {
        float shineDamper;

        float reflectivity;

        MaterialUniform() = default;

        MaterialUniform(const float shineDaperr, const float reflect)
            : shineDamper(shineDaperr)
            , reflectivity(reflect)
        {
        }
    };

    struct alignas(16) UniformsVS
    {
        alignas(16) glm::mat4 bones[MAX_BONES_COUNT];

        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;

        alignas(16) glm::mat4 normalMatrix;

        alignas(16) glm::vec4 clipPlane;

        alignas(16) glm::vec4 cameraPosition;

        alignas(16) LightningUniform lightning;

        alignas(16) glm::vec4 textureOffset;

        alignas(16) uint32_t textureNumberOfRows;
        uint32_t useFakeLightning;
        float density;
        float gradient;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) ShadowsUniform shadows;

        alignas(16) LightningUniform lightning;

        alignas(16) MaterialUniform material;

        alignas(16) glm::vec4 fogColor;

        alignas(16) glm::vec4 selectedColor;

        alignas(16) uint32_t selected;
        uint32_t castedByShadows;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsFS> > m_uniformsPoolFS;
};
} // namespace prev_test::render::renderer::animation

#endif