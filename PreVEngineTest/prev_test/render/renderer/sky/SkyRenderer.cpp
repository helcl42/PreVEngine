#include "SkyRenderer.h"

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/sky/ISkyComponent.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky {
SkyRenderer::SkyRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void SkyRenderer::Init()
{
    // compute sky
    // clang-format off
    m_skyShader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_COMPUTE_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sky_comp.spv") }
        })
        .AddDescriptorSets({
            { "outFragColor", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "outBloom", 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "outAlphaness", 2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "outCloudDistance", 3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "perlinNoiseTex", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "weatherTex", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "uboCS", 6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT }
        })
        .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Sky Compute Shader created");

    // clang-format off
    m_skyPipeline = prev::render::pipeline::ComputePipelineBuilder{ m_device, *m_skyShader }
        .Build();
    // clang-format on

    LOGI("Sky Compute Pipeline created");

    m_uniformsPoolSkyCS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsSkyCS>>(m_allocator);
    m_uniformsPoolSkyCS->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    // compute sky post process
    // clang-format off
    m_skyPostProcessShader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_COMPUTE_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sky_post_process_comp.spv") }
        })
        .AddDescriptorSets({
            { "outFragColor", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "skyTex", 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "bloomTex", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "uboCS", 3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT }
        })
        .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Sky PostProcess Compute Shader created");

    // clang-format off
    m_skyPostProcessPipeline = prev::render::pipeline::ComputePipelineBuilder{ m_device, *m_skyPostProcessShader }
        .Build();
    // clang-format on

    LOGI("Sky PostProcess Compute Pipeline created");

    m_uniformsPoolSkyPostProcessCS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsSkyPostProcessCS>>(m_allocator);
    m_uniformsPoolSkyPostProcessCS->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    // compositor
    // clang-format off
    m_compositeShader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sky_composite_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sky_composite_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "colorTex", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_VIEW_COUNT, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "depthTex", 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_VIEW_COUNT, VK_SHADER_STAGE_FRAGMENT_BIT },
        })
        .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Sky Composite Shader created");

    // clang-format off
    m_compositePipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_compositeShader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_BACK_BIT)
        .Build();
    // clang-format on

    LOGI("Sky Composite Pipeline created");
}

void SkyRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
    const auto skyComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::sky::ISkyComponent>(m_scene.GetRootNode(), { TAG_SKY_RENDER_COMPONENT });
    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_MAIN_LIGHT });

    // generate clouds using compute queue
    const VkExtent2D extent{ renderContext.rect.extent.width - renderContext.rect.offset.x, renderContext.rect.extent.height - renderContext.rect.offset.y };

    for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
        // TODO - put this work on dedicated compute queue if available, double buffering, ...

        auto& skyColorImageBuffer{ m_skyColorImageBuffer[viewIndex] };
        auto& skyBloomImageBuffer{ m_skyBloomImageBuffer[viewIndex] };
        auto& skyAlphanessImageBuffer{ m_skyAlphanessImageBuffer[viewIndex] };
        auto& skyCloudDistanceImageBuffer{ m_skyCloudDistanceImageBuffer[viewIndex] };
        auto& skyPostProcessColorImageBuffer{ m_skyPostProcessColorImageBuffer[viewIndex] };

        auto& skyColorImageSampler{ m_skyColorImageSampler[viewIndex] };
        auto& skyBloomImageSampler{ m_skyBloomImageSampler[viewIndex] };
        auto& skyAlphanessImageSampler{ m_skyAlphanessImageSampler[viewIndex] };
        auto& skyCloudDistanceImageSampler{ m_skyCloudDistanceImageSampler[viewIndex] };
        auto& skyPostProcessImageSampler{ m_skyPostProcessImageSampler[viewIndex] };

        UpdateImageBufferExtents(extent, COLOR_FORMAT, skyColorImageBuffer, skyColorImageSampler);
        UpdateImageBufferExtents(extent, COLOR_FORMAT, skyBloomImageBuffer, skyBloomImageSampler);
        UpdateImageBufferExtents(extent, COLOR_FORMAT, skyAlphanessImageBuffer, skyAlphanessImageSampler);
        UpdateImageBufferExtents(extent, DEPTH_FORMAT, skyCloudDistanceImageBuffer, skyCloudDistanceImageSampler);
        UpdateImageBufferExtents(extent, COLOR_FORMAT, skyPostProcessColorImageBuffer, skyPostProcessImageSampler);

        // regular sky render
        skyColorImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_GENERAL, renderContext.commandBuffer);
        skyBloomImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_GENERAL, renderContext.commandBuffer);
        skyAlphanessImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_GENERAL, renderContext.commandBuffer);
        skyCloudDistanceImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_GENERAL, renderContext.commandBuffer);

        const float skyNearClippingPlane{ std::min(renderContext.nearFarClippingPlanes[viewIndex].y * 100.0f, 10.0f) };
        const float skyFarClippingPlane{ renderContext.nearFarClippingPlanes[viewIndex].y };

        const auto fov{ prev::util::math::CreateFovFromProjectionMatrix(renderContext.projectionMatrices[viewIndex]) };
        const prev_test::render::ViewFrustum skyViewFrustum(fov.angleLeft, fov.angleRight, fov.angleUp, fov.angleDown, skyNearClippingPlane, skyFarClippingPlane);
        const auto projectionMatrix{ skyViewFrustum.CreateProjectionMatrix() };

        // const auto projectionMatrix{ renderContext.projectionMatrices[viewIndex] };

        auto uboCS = m_uniformsPoolSkyCS->GetNext();

        UniformsSkyCS uniformsCS{};
        uniformsCS.resolution = glm::vec4(extent.width, extent.height, 0.0f, 0.0f);
        uniformsCS.projectionMatrix = projectionMatrix;
        uniformsCS.inverseProjectionMatrix = glm::inverse(projectionMatrix);
        uniformsCS.viewMatrix = renderContext.viewMatrices[viewIndex];
        uniformsCS.inverseViewMatrix = glm::inverse(renderContext.viewMatrices[viewIndex]);
        uniformsCS.lightColor = glm::vec4(mainLightComponent->GetColor(), 1.0f);
        uniformsCS.lightDirection = glm::vec4(-mainLightComponent->GetDirection(), 0.0f);
        uniformsCS.cameraPosition = glm::vec4(renderContext.cameraPositions[viewIndex], 1.0f);
        uniformsCS.baseCloudColor = glm::vec4(skyComponent->GetCloudBaseColor(), 1.0f);
        uniformsCS.skyColorBottom = glm::vec4(skyComponent->GetBottomColor(), 1.0f);
        uniformsCS.skyColorTop = glm::vec4(skyComponent->GetTopColor(), 1.0f);
        uniformsCS.windDirection = glm::normalize(glm::vec4(0.5f, 0.0f, 0.1f, 0.0f));
        uniformsCS.worldOrigin = glm::vec4(renderContext.cameraPositions[0].x, 0.0f, renderContext.cameraPositions[0].z, 1.0f);
        uniformsCS.time = skyComponent->GetElapsedTime();
        uniformsCS.coverageFactor = 0.45f;
        uniformsCS.cloudSpeed = 450.0f;
        uniformsCS.crispiness = 40.0f;
        uniformsCS.absorption = 0.0035f;
        uniformsCS.curliness = 0.1f;
        uniformsCS.enablePowder = 0;
        uniformsCS.densityFactor = 0.02f;
        uniformsCS.earthRadius = 600000.0f;
        uniformsCS.sphereInnerRadius = uniformsCS.earthRadius + 5000.0f;
        uniformsCS.sphereOuterRadius = uniformsCS.sphereInnerRadius + 17000.0f;
        uniformsCS.cloudTopOffset = 750.0f;
        uniformsCS.maxDepth = MAX_DEPTH;

        uboCS->Data(uniformsCS);

        m_skyShader->Bind("uboCS", *uboCS);

        m_skyShader->Bind("perlinNoiseTex", *skyComponent->GetPerlinWorleyNoise(), *skyComponent->GetPerlinWorleyNoiseSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_skyShader->Bind("weatherTex", *skyComponent->GetWeather(), *skyComponent->GetWeatherSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        m_skyShader->Bind("outFragColor", *skyColorImageBuffer, *skyColorImageSampler, VK_IMAGE_LAYOUT_GENERAL);
        m_skyShader->Bind("outBloom", *skyBloomImageBuffer, *skyBloomImageSampler, VK_IMAGE_LAYOUT_GENERAL);
        m_skyShader->Bind("outAlphaness", *skyAlphanessImageBuffer, *skyAlphanessImageSampler, VK_IMAGE_LAYOUT_GENERAL);
        m_skyShader->Bind("outCloudDistance", *skyCloudDistanceImageBuffer, *skyCloudDistanceImageSampler, VK_IMAGE_LAYOUT_GENERAL);

        const VkDescriptorSet descriptorSetCompute = m_skyShader->UpdateNextDescriptorSet();

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_skyPipeline);
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_skyPipeline->GetLayout(), 0, 1, &descriptorSetCompute, 0, 0);

        vkCmdDispatch(renderContext.commandBuffer, prev::util::vk::GetComputeGroupSize(extent.width, 16), prev::util::vk::GetComputeGroupSize(extent.height, 16), 1);

        skyColorImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, renderContext.commandBuffer);
        skyBloomImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, renderContext.commandBuffer);
        skyAlphanessImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, renderContext.commandBuffer);
        skyCloudDistanceImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, renderContext.commandBuffer);

        // sky post process render
        skyPostProcessColorImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_GENERAL, renderContext.commandBuffer);

        auto uboPostCS = m_uniformsPoolSkyPostProcessCS->GetNext();

        glm::vec4 lightPositionClipSpace = renderContext.projectionMatrices[viewIndex] * renderContext.viewMatrices[viewIndex] * glm::vec4(mainLightComponent->GetPosition(), 1.0f);
        glm::vec3 lightPositionNdc = lightPositionClipSpace / lightPositionClipSpace.w;
        glm::vec2 lightPositionO1 = lightPositionNdc * 0.5f + 0.5f;

        UniformsSkyPostProcessCS uniformsPostCS{};
        uniformsPostCS.resolution = glm::vec4(extent.width, extent.height, 0.0f, 0.0f);
        uniformsPostCS.lisghtPosition = glm::vec4(lightPositionO1, 0.0f, 1.0f);
        uniformsPostCS.enableGodRays = 1;
        uniformsPostCS.lightDotCameraForward = glm::dot(glm::normalize(renderContext.cameraPositions[viewIndex] - mainLightComponent->GetPosition()),
            glm::normalize(prev::util::math::GetForwardVector(renderContext.viewMatrices[viewIndex])));

        uboPostCS->Data(uniformsPostCS);

        m_skyPostProcessShader->Bind("uboCS", *uboPostCS);

        m_skyPostProcessShader->Bind("skyTex", *skyColorImageBuffer, *skyColorImageSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_skyPostProcessShader->Bind("bloomTex", *skyBloomImageBuffer, *skyBloomImageSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        m_skyPostProcessShader->Bind("outFragColor", *skyPostProcessColorImageBuffer, *skyPostProcessImageSampler, VK_IMAGE_LAYOUT_GENERAL);

        const VkDescriptorSet descriptorSetComputePost = m_skyPostProcessShader->UpdateNextDescriptorSet();

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_skyPostProcessPipeline);
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_skyPostProcessPipeline->GetLayout(), 0, 1, &descriptorSetComputePost, 0, 0);

        vkCmdDispatch(renderContext.commandBuffer, prev::util::vk::GetComputeGroupSize(extent.width, 16), prev::util::vk::GetComputeGroupSize(extent.height, 16), 1);

        skyPostProcessColorImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, renderContext.commandBuffer);
    }
}

void SkyRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_compositePipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SkyRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_SKY_RENDER_COMPONENT })) {
        return;
    }

    const auto skyComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::sky::ISkyComponent>(node);

    for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
        auto& skyCloudDistanceImageBuffer{ m_skyCloudDistanceImageBuffer[viewIndex] };
        auto& skyPostProcessColorImageBuffer{ m_skyPostProcessColorImageBuffer[viewIndex] };

        auto& skyCloudDistanceImageSampler{ m_skyCloudDistanceImageSampler[viewIndex] };
        auto& skyPostProcessImageSampler{ m_skyPostProcessImageSampler[viewIndex] };
#ifdef ENABLE_XR
        const auto colorTexKey{ "colorTex[" + std::to_string(viewIndex) + "]" };
        const auto depthTexKey{ "depthTex[" + std::to_string(viewIndex) + "]" };
#else
        const auto colorTexKey{ "colorTex" };
        const auto depthTexKey{ "depthTex" };
#endif
        m_compositeShader->Bind(colorTexKey, *skyPostProcessColorImageBuffer, *skyPostProcessImageSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_compositeShader->Bind(depthTexKey, *skyCloudDistanceImageBuffer, *skyCloudDistanceImageSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    const VkDescriptorSet descriptorSet = m_compositeShader->UpdateNextDescriptorSet();
    const VkBuffer vertexBuffers[] = { *skyComponent->GetModel()->GetVertexBuffer() };
    const VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(renderContext.commandBuffer, *skyComponent->GetModel()->GetIndexBuffer(), 0, skyComponent->GetModel()->GetIndexBuffer()->GetIndexType());
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_compositePipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(renderContext.commandBuffer, skyComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
}

void SkyRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SkyRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void SkyRenderer::ShutDown()
{
    for (uint32_t viewIndex = 0; viewIndex < MAX_VIEW_COUNT; ++viewIndex) {
        m_skyPostProcessColorImageBuffer[viewIndex] = nullptr;
        m_skyCloudDistanceImageBuffer[viewIndex] = nullptr;
        m_skyAlphanessImageBuffer[viewIndex] = nullptr;
        m_skyBloomImageBuffer[viewIndex] = nullptr;
        m_skyColorImageBuffer[viewIndex] = nullptr;

        m_skyPostProcessImageSampler[viewIndex] = nullptr;
        m_skyCloudDistanceImageSampler[viewIndex] = nullptr;
        m_skyAlphanessImageSampler[viewIndex] = nullptr;
        m_skyBloomImageSampler[viewIndex] = nullptr;
        m_skyColorImageSampler[viewIndex] = nullptr;
    }

    m_compositePipeline = nullptr;
    m_compositeShader = nullptr;

    m_skyPostProcessPipeline = nullptr;
    m_skyPostProcessShader = nullptr;

    m_skyPipeline = nullptr;
    m_skyShader = nullptr;
}

void SkyRenderer::UpdateImageBufferExtents(const VkExtent2D& extent, const VkFormat format, std::shared_ptr<prev::render::buffer::ImageBuffer>& imageBuffer, std::shared_ptr<prev::render::sampler::Sampler>& sampler)
{
    if (imageBuffer == nullptr || imageBuffer->GetExtent().width != extent.width || imageBuffer->GetExtent().height != extent.height) {
        const auto formatProperties{ prev::util::vk::GetFormatProperties(m_device.GetGPU(), format) };
        const auto samplingFilter{ (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) ? VK_FILTER_LINEAR : VK_FILTER_NEAREST };

        imageBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                          .SetExtent({ extent.width, extent.height, 1 })
                          .SetFormat(format)
                          .SetType(VK_IMAGE_TYPE_2D)
                          .SetUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
                          .SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                          .Build();
        sampler = std::make_shared<prev::render::sampler::Sampler>(m_device, static_cast<float>(imageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, samplingFilter, samplingFilter, VK_SAMPLER_MIPMAP_MODE_NEAREST);
    }
}
} // namespace prev_test::render::renderer::sky
