#ifndef __TEXTURELESS_RENDERER_H__
#define __TEXTURELESS_RENDERER_H__

#include "../../IMesh.h"
#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"
#include "../../../component/light/LightCommon.h"
#include "../../../component/shadow/ShadowsCommon.h"

#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::normal {
class TexturelessRenderer final : public IRenderer<NormalRenderContext> {
public:
    TexturelessRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~TexturelessRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const NormalRenderContext& renderContext) override;

    void PreRender(const NormalRenderContext& renderContext) override;

    void Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const NormalRenderContext& renderContext) override;

    void AfterRender(const NormalRenderContext& renderContext) override;

    void ShutDown() override;

private:
    void RenderMeshNode(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev_test::render::MeshNode& meshNode);

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
        ShadowsCascadeUniform cascades[prev_test::component::shadow::CASCADES_COUNT];

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
        LightUniform lights[prev_test::component::light::MAX_LIGHT_COUNT];

        uint32_t realCountOfLights;

        float ambientFactor;
    };

    struct MaterialUniform {
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

        DEFAULT_ALIGNMENT glm::vec4 textureOffset;

        DEFAULT_ALIGNMENT uint32_t textureNumberOfRows;
        uint32_t useFakeLightning;
        float density;
        float gradient;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT ShadowsUniform shadows;

        DEFAULT_ALIGNMENT LightningUniform lightning;

        DEFAULT_ALIGNMENT MaterialUniform material;

        DEFAULT_ALIGNMENT glm::vec4 fogColor;

        DEFAULT_ALIGNMENT glm::vec4 selectedColor;

        DEFAULT_ALIGNMENT uint32_t selected;
        uint32_t castedByShadows;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsVS>> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsFS>> m_uniformsPoolFS;
};
} // namespace prev_test::render::renderer::normal

#endif // !__TEXTURELESS_RENDERER_H__
