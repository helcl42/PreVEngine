#ifndef __TERRAIN_PARALLAX_MAPPED_RENDERER_H__
#define __TERRAIN_PARALLAX_MAPPED_RENDERER_H__

#include "../../pipeline/IPipeline.h"
#include "../RenderContextUserData.h"

#include "../../../General.h"
#include "../../../component/light/LightCommon.h"
#include "../../../component/shadow/ShadowsCommon.h"

#include <prev/render/IRenderer.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::terrain {
class TerrainParallaxMappedRenderer final : public prev::render::IRenderer<NormalRenderContextUserData> {
public:
    TerrainParallaxMappedRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~TerrainParallaxMappedRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    struct DEFAULT_ALIGNMENT ShadowsCascadeUniform {
        glm::mat4 viewProjectionMatrix;

        glm::vec4 split;

        ShadowsCascadeUniform() = default;

        ShadowsCascadeUniform(const glm::mat4& vpMat, const glm::vec4& spl)
            : viewProjectionMatrix(vpMat)
            , split(spl)
        {
        }
    };

    struct DEFAULT_ALIGNMENT ShadowsUniform {
        ShadowsCascadeUniform cascades[prev_test::component::shadow::CASCADES_COUNT];

        uint32_t enabled;
    };

    struct DEFAULT_ALIGNMENT LightUniform {
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

    struct DEFAULT_ALIGNMENT LightningUniform {
        LightUniform lights[prev_test::component::light::MAX_LIGHT_COUNT];

        uint32_t realCountOfLights;

        float ambientFactor;
    };

    struct DEFAULT_ALIGNMENT MaterialUniform {
        glm::vec4 color;

        float shineDamper;

        float reflectivity;

        MaterialUniform() = default;

        MaterialUniform(const glm::vec4& col, const float shineDaperr, const float reflect)
            : color(col)
            , shineDamper(shineDaperr)
            , reflectivity(reflect)
        {
        }
    };

    struct DEFAULT_ALIGNMENT UniformsVS {
        DEFAULT_ALIGNMENT glm::mat4 modelMatrix;

        DEFAULT_ALIGNMENT glm::mat4 viewMatrix;

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrix;

        DEFAULT_ALIGNMENT glm::mat4 normalMatrix;

        DEFAULT_ALIGNMENT glm::vec4 clipPlane;

        DEFAULT_ALIGNMENT glm::vec4 cameraPosition;

        DEFAULT_ALIGNMENT LightningUniform lightning;

        DEFAULT_ALIGNMENT float density;
        float gradient;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT ShadowsUniform shadows;

        DEFAULT_ALIGNMENT LightningUniform lightning;

        DEFAULT_ALIGNMENT MaterialUniform material[4];

        DEFAULT_ALIGNMENT glm::vec4 fogColor;

        DEFAULT_ALIGNMENT glm::vec4 selectedColor;

        DEFAULT_ALIGNMENT uint32_t selected;
        uint32_t castedByShadows;
        float minHeight;
        float maxHeight;

        DEFAULT_ALIGNMENT glm::vec4 heightSteps[4];
        DEFAULT_ALIGNMENT glm::vec4 heightScale[4];

        DEFAULT_ALIGNMENT float heightTransitionRange;
        uint32_t numLayers;
        uint32_t mappingMode;

        DEFAULT_ALIGNMENT float maxAngleToFallback;
    };

private:
    const uint32_t m_descriptorCount{ 3000 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsVS>> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsFS>> m_uniformsPoolFS;
};
} // namespace prev_test::render::renderer::terrain

#endif