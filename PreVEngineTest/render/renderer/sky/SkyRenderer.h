#ifndef __SKY_RENDERER_H__
#define __SKY_RENDERER_H__

#include "../../../General.h"
#include "../../pipeline/IPipeline.h"
#include "../RenderContextUserData.h"

#include "../../../component/common/IOffScreenRenderPassComponent.h"

#include <prev/render/IRenderer.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/buffer/image/IImageBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::sky {
class SkyRenderer final : public prev::render::IRenderer<NormalRenderContextUserData> {
public:
    SkyRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~SkyRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    bool UpdateOffScreenRenderTarget(const VkExtent2D& extent, const VkFormat& depthFormat, const std::vector<VkFormat>& colorFormats, std::shared_ptr<prev_test::component::common::IOffScreenRenderPassComponent>& offScreenRenderTarget);

private:
    struct DEFAULT_ALIGNMENT UniformsSkyFS {
        DEFAULT_ALIGNMENT glm::vec4 resolution;

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 inverseProjectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 viewMatrix;
        DEFAULT_ALIGNMENT glm::mat4 inverseViewMatrix;

        DEFAULT_ALIGNMENT glm::vec4 lightColor;
        DEFAULT_ALIGNMENT glm::vec4 lightDirection;
        DEFAULT_ALIGNMENT glm::vec4 cameraPosition;

        DEFAULT_ALIGNMENT glm::vec4 baseCloudColor;
        DEFAULT_ALIGNMENT glm::vec4 skyColorBottom;
        DEFAULT_ALIGNMENT glm::vec4 skyColorTop;
        DEFAULT_ALIGNMENT glm::vec4 windDirection;

        DEFAULT_ALIGNMENT float time;
        float coverageFactor;
        float cloudSpeed;
        float crispiness;

        DEFAULT_ALIGNMENT float absorption;
        float curliness;
        uint32_t enablePowder;
        float densityFactor;

        DEFAULT_ALIGNMENT float earthRadius;
        float sphereInnerRadius;
        float sphereOuterRadius;
        float cloudTopOffset;
    };

    struct DEFAULT_ALIGNMENT UniformsSkyPostProcessFS {
        DEFAULT_ALIGNMENT glm::vec4 resolution;
        DEFAULT_ALIGNMENT glm::vec4 lisghtPosition;
        DEFAULT_ALIGNMENT uint32_t enableGodRays;
        float lightDotCameraFront;
    };

private:
    const uint32_t m_descriptorCount{ 10 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    // sky
    std::unique_ptr<prev::render::shader::Shader> m_skyShader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_skyPipeline;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsSkyFS>> m_uniformsPoolSkyFS;

    // sky post-process
    std::unique_ptr<prev::render::shader::Shader> m_skyPostProcessShader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_skyPostProcessPipeline;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsSkyPostProcessFS>> m_uniformsPoolSkyPostProcessFS;

    // composite render
    std::unique_ptr<prev::render::shader::Shader> m_skyCompositeShader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_skyCompositePipeline;

    // off-screen render target
    std::shared_ptr<prev_test::component::common::IOffScreenRenderPassComponent> m_skyOffScreenRenderTarget;

    static const inline VkFormat m_skyOffScreenRenderTargetDepthFormat{ VK_FORMAT_D32_SFLOAT };

    static const inline std::vector<VkFormat> m_skyOffScreenRenderTargetColorFormats{ VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM };

    // off-screen post-process render target
    std::shared_ptr<prev_test::component::common::IOffScreenRenderPassComponent> m_skyPostProcessOffScreenRenderTarget;

    static const inline VkFormat m_skyPostProcessOffScreenRenderTargetDepthFormat{ VK_FORMAT_UNDEFINED };

    static const inline std::vector<VkFormat> m_skyPostProcessOffScreenRenderTargetColorFormats{ VK_FORMAT_B8G8R8A8_UNORM };
};
} // namespace prev_test::render::renderer::sky

#endif // !__SKY_RENDERER_H__
