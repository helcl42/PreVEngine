#include "SkyRenderer.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/sky/ISkyComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/pipeline/ComputePipelineBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/MathUtils.h>

namespace prev_test::render::renderer::sky {
SkyRenderer::SkyRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
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
            { GFX_SHADER_STAGE_COMPUTE, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/sky_comp") }
        })
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::StorageTexture("outFragColor", 0, GFX_SHADER_STAGE_COMPUTE, COLOR_FORMAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::StorageTexture("outBloom", 1, GFX_SHADER_STAGE_COMPUTE, COLOR_FORMAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::StorageTexture("outAlphaness", 2, GFX_SHADER_STAGE_COMPUTE, COLOR_FORMAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::StorageTexture("outCloudDistance", 3, GFX_SHADER_STAGE_COMPUTE, DEPTH_FORMAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("perlinNoiseTex", 4, GFX_SHADER_STAGE_COMPUTE, GFX_TEXTURE_VIEW_TYPE_3D),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("perlinNoiseSampler", 5, GFX_SHADER_STAGE_COMPUTE),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("weatherTex", 6, GFX_SHADER_STAGE_COMPUTE, GFX_TEXTURE_VIEW_TYPE_2D),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("weatherSampler", 7, GFX_SHADER_STAGE_COMPUTE),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboCS", 8, GFX_SHADER_STAGE_COMPUTE),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("prevColorTex", 9, GFX_SHADER_STAGE_COMPUTE, GFX_TEXTURE_VIEW_TYPE_2D),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("prevColorSampler", 10, GFX_SHADER_STAGE_COMPUTE),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("prevDepthTex", 11, GFX_SHADER_STAGE_COMPUTE, GFX_TEXTURE_VIEW_TYPE_2D, 1, GFX_TEXTURE_SAMPLE_TYPE_UNFILTERABLE_FLOAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("prevDepthSampler", 12, GFX_SHADER_STAGE_COMPUTE, true)
        })
        .Build();
    // clang-format on

    LOGI("Sky Compute Shader created");

    // clang-format off
    m_skyPipeline = prev::render::pipeline::ComputePipelineBuilder{ m_device, *m_skyShader }
        .Build();
    // clang-format on

    LOGI("Sky Compute Pipeline created");

    m_uniformsPoolSkyCS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                              .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                              .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                              .SetChunkSize(m_descriptorCount)
                              .SetStride(sizeof(UniformsSkyCS))
                              .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                              .BuildFrameScoped();

    // compute sky post process
    // clang-format off
    m_skyPostProcessShader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_COMPUTE, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/sky_post_process_comp") }
        })
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::StorageTexture("outFragColor", 0, GFX_SHADER_STAGE_COMPUTE, COLOR_FORMAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("skyTex", 1, GFX_SHADER_STAGE_COMPUTE, GFX_TEXTURE_VIEW_TYPE_2D),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("skySampler", 2, GFX_SHADER_STAGE_COMPUTE),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("bloomTex", 3, GFX_SHADER_STAGE_COMPUTE, GFX_TEXTURE_VIEW_TYPE_2D),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("bloomSampler", 4, GFX_SHADER_STAGE_COMPUTE),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboCS", 5, GFX_SHADER_STAGE_COMPUTE)
        })
        .Build();
    // clang-format on

    LOGI("Sky PostProcess Compute Shader created");

    // clang-format off
    m_skyPostProcessPipeline = prev::render::pipeline::ComputePipelineBuilder{ m_device, *m_skyPostProcessShader }
        .Build();
    // clang-format on

    LOGI("Sky PostProcess Compute Pipeline created");

    m_uniformsPoolSkyPostProcessCS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                                         .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                                         .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                                         .SetChunkSize(m_descriptorCount)
                                         .SetStride(sizeof(UniformsSkyPostProcessCS))
                                         .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                                         .BuildFrameScoped();

    // compositor
    // clang-format off
    m_compositeShader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/sky_composite_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/sky_composite_frag") }
        })
        .AddVertexInputAttributes({
            prev::render::shader::VertexInputAttribute{ 0, 0, GFX_FORMAT_R32G32B32_FLOAT, 0 },
            prev::render::shader::VertexInputAttribute{ 0, 1, GFX_FORMAT_R32G32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })},
            prev::render::shader::VertexInputAttribute{ 0, 2, GFX_FORMAT_R32G32B32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })}
        })
        .AddVertexInputBindings({
            prev::render::shader::VertexInputBinding{ 0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), GFX_VERTEX_STEP_MODE_VERTEX }
        })
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("colorTex", 0, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D, MAX_VIEW_COUNT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("colorSampler", 1, GFX_SHADER_STAGE_FRAGMENT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("depthTex", 2, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D, MAX_VIEW_COUNT, GFX_TEXTURE_SAMPLE_TYPE_UNFILTERABLE_FLOAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("depthSampler", 3, GFX_SHADER_STAGE_FRAGMENT, true)
        })
        .Build();
    // clang-format on

    LOGI("Sky Composite Shader created");

    // clang-format off
    m_compositePipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_compositeShader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_FILL)
        .SetCullingMode(GFX_CULL_MODE_NONE)
        .Build();
    // clang-format on

    LOGI("Sky Composite Pipeline created");

    m_samplers[SamplerType::LINEAR] = prev::render::sampler::SamplerBuilder{ m_device }
                                          .SetMipMapMode(GFX_FILTER_MODE_NEAREST)
                                          .Build();
    m_samplers[SamplerType::NEAREST] = prev::render::sampler::SamplerBuilder{ m_device }
                                           .SetMinFilter(GFX_FILTER_MODE_NEAREST)
                                           .SetMagFilter(GFX_FILTER_MODE_NEAREST)
                                           .SetMipMapMode(GFX_FILTER_MODE_NEAREST)
                                           .Build();
    m_samplers[SamplerType::LINEAR_REPEATED] = prev::render::sampler::SamplerBuilder{ m_device }
                                                   .SetMipMapMode(GFX_FILTER_MODE_LINEAR)
                                                   .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                                                   .Build();

    LOGI("Sky Samplers created");
}

void SkyRenderer::BeginFrame(const NormalRenderContext& renderContext)
{
    m_skyShader->BeginFrame(renderContext.frameInFlightIndex);
    m_skyPostProcessShader->BeginFrame(renderContext.frameInFlightIndex);
    m_compositeShader->BeginFrame(renderContext.frameInFlightIndex);
    m_uniformsPoolSkyCS->BeginFrame(renderContext.frameInFlightIndex);
    m_uniformsPoolSkyPostProcessCS->BeginFrame(renderContext.frameInFlightIndex);

    const auto skyComponent = prev::scene::component::NodeComponentHelper::Find<prev_test::component::sky::ISkyComponent>(m_scene.GetRootNode(), { TAG_SKY_RENDER_COMPONENT });
    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::Find<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_MAIN_LIGHT });

    const float viewportScale{ 1.0f };

    // generate clouds using compute queue
    const GfxExtent2D extent{ static_cast<uint32_t>((renderContext.rect.extent.width - renderContext.rect.origin.x) * viewportScale), static_cast<uint32_t>((renderContext.rect.extent.height - renderContext.rect.origin.y) * viewportScale) };

    for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
        // TODO - put this work on dedicated compute queue if available, double buffering, ...

        auto& skyColorImageBuffer{ m_skyColorImageBuffer[viewIndex] };
        auto& skyBloomImageBuffer{ m_skyBloomImageBuffer[viewIndex] };
        auto& skyAlphanessImageBuffer{ m_skyAlphanessImageBuffer[viewIndex] };
        auto& skyCloudDistanceImageBuffer{ m_skyCloudDistanceImageBuffer[viewIndex] };
        auto& skyPostProcessColorImageBuffer{ m_skyPostProcessColorImageBuffer[viewIndex] };

        UpdateImageBufferExtents(renderContext.commandEncoder, extent, COLOR_FORMAT, skyColorImageBuffer, GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_STORAGE_BINDING | GFX_TEXTURE_USAGE_COPY_SRC);
        UpdateImageBufferExtents(renderContext.commandEncoder, extent, COLOR_FORMAT, skyBloomImageBuffer, GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_STORAGE_BINDING);
        UpdateImageBufferExtents(renderContext.commandEncoder, extent, COLOR_FORMAT, skyAlphanessImageBuffer, GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_STORAGE_BINDING);
        UpdateImageBufferExtents(renderContext.commandEncoder, extent, DEPTH_FORMAT, skyCloudDistanceImageBuffer, GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_STORAGE_BINDING | GFX_TEXTURE_USAGE_COPY_SRC);
        UpdateImageBufferExtents(renderContext.commandEncoder, extent, COLOR_FORMAT, skyPostProcessColorImageBuffer, GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_STORAGE_BINDING);

        // History buffers for reprojection
        static constexpr uint32_t REPROJ_WARMUP_FRAMES = 5;
        bool historyValid = m_frameCounter >= REPROJ_WARMUP_FRAMES && m_skyHistoryColorImageBuffer[viewIndex].image != nullptr && m_skyHistoryColorImageBuffer[viewIndex].image->GetExtent().width == extent.width && m_skyHistoryColorImageBuffer[viewIndex].image->GetExtent().height == extent.height;
        UpdateImageBufferExtents(renderContext.commandEncoder, extent, COLOR_FORMAT, m_skyHistoryColorImageBuffer[viewIndex], GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_COPY_DST);
        UpdateImageBufferExtents(renderContext.commandEncoder, extent, DEPTH_FORMAT, m_skyHistoryDepthImageBuffer[viewIndex], GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_COPY_DST);

        auto& skyColorImageSampler{ m_samplers[skyColorImageBuffer.samplerType] };
        auto& skyBloomImageSampler{ m_samplers[skyBloomImageBuffer.samplerType] };
        auto& skyAlphanessImageSampler{ m_samplers[skyAlphanessImageBuffer.samplerType] };
        auto& skyCloudDistanceImageSampler{ m_samplers[skyCloudDistanceImageBuffer.samplerType] };
        auto& skyPostProcessImageSampler{ m_samplers[skyPostProcessColorImageBuffer.samplerType] };

        auto& linearRepeatedSampler{ m_samplers[SamplerType::LINEAR_REPEATED] };

        // regular sky render
        skyColorImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_GENERAL, renderContext.commandEncoder);
        skyBloomImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_GENERAL, renderContext.commandEncoder);
        skyAlphanessImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_GENERAL, renderContext.commandEncoder);
        skyCloudDistanceImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_GENERAL, renderContext.commandEncoder);

        const float skyNearClippingPlane{ std::min(renderContext.nearFarClippingPlanes[viewIndex].y * 100.0f, 10.0f) };
        const float skyFarClippingPlane{ renderContext.nearFarClippingPlanes[viewIndex].y };

        const auto fov{ prev::util::math::CreateFovFromProjectionMatrix(renderContext.projectionMatrices[viewIndex]) };
        const prev_test::render::ViewFrustum skyViewFrustum(fov.angleLeft, fov.angleRight, fov.angleUp, fov.angleDown, skyNearClippingPlane, skyFarClippingPlane);
        const auto projectionMatrix{ skyViewFrustum.CreateProjectionMatrix() };
        // const auto projectionMatrix{ renderContext.projectionMatrices[viewIndex] };

        const uint32_t enableCheckerboard{ 0 };
        const uint32_t enableTemporalBlend{ 0 };
        const uint32_t enableFullReproject{ 1 };
        const float reprojectionBlend{ 0.9f };

        auto& uboCS = m_uniformsPoolSkyCS->Next();

        UniformsSkyCS uniformsCS{};
        // View & Matrices
        uniformsCS.resolution = glm::vec4(extent.width, extent.height, 0.0f, 0.0f);
        uniformsCS.projectionMatrix = projectionMatrix;
        uniformsCS.inverseProjectionMatrix = glm::inverse(projectionMatrix);
        uniformsCS.viewMatrix = renderContext.viewMatrices[viewIndex];
        uniformsCS.inverseViewMatrix = glm::inverse(renderContext.viewMatrices[viewIndex]);

        // Light & Color
        uniformsCS.lightColor = glm::vec4(mainLightComponent->GetColor(), 1.0f);
        uniformsCS.lightDirection = glm::vec4(-mainLightComponent->GetDirection(), 0.0f);
        uniformsCS.baseCloudColor = glm::vec4(skyComponent->GetCloudBaseColor(), 1.0f);
        uniformsCS.skyColorBottom = glm::vec4(skyComponent->GetBottomColor(), 1.0f);
        uniformsCS.skyColorTop = glm::vec4(skyComponent->GetTopColor(), 1.0f);

        // Camera & World
        uniformsCS.cameraPosition = glm::vec4(renderContext.cameraPositions[viewIndex], 1.0f);
        uniformsCS.worldOrigin = glm::vec4(renderContext.cameraPositions[0].x, 0.0f, renderContext.cameraPositions[0].z, 1.0f);
        uniformsCS.windDirection = glm::normalize(glm::vec4(0.5f, 0.0f, 0.1f, 0.0f));
        uniformsCS.earthRadius = 600000.0f;
        uniformsCS.sphereInnerRadius = uniformsCS.earthRadius + 5000.0f;
        uniformsCS.sphereOuterRadius = uniformsCS.sphereInnerRadius + 17000.0f;
        uniformsCS.cloudTopOffset = 750.0f;

        // Cloud Shape
        uniformsCS.coverageFactor = 0.42f;
        uniformsCS.crispiness = 37.0f;
        uniformsCS.curliness = 0.102f;
        uniformsCS.densityFactor = 0.016f;
        uniformsCS.cloudSpeed = 450.0f;
        uniformsCS.absorption = 0.0029f;
        uniformsCS.maxDepth = MAX_DEPTH;
        uniformsCS.time = skyComponent->GetElapsedTime();

        // Rendering Quality
        uniformsCS.cloudSteps = 80;
        uniformsCS.lightSteps = 6;
        uniformsCS.useIGN = 1;
        uniformsCS.frameCounter = m_frameCounter;

        // Effects
        uniformsCS.enablePowder = 1;
        uniformsCS.powderWeight = 0.15f;
        uniformsCS.lodScale = 2.0f;
        uniformsCS.ambientScale = 1.35f;

        // Reprojection
        uniformsCS.prevViewProjectionMatrix = m_prevViewProjectionMatrix[viewIndex];
        uniformsCS.currentViewProjectionMatrix = projectionMatrix * renderContext.viewMatrices[viewIndex];
        uniformsCS.prevInverseViewProjectionMatrix = glm::inverse(m_prevViewProjectionMatrix[viewIndex]);
        uniformsCS.enableCheckerboard = (enableCheckerboard && historyValid) ? 1 : 0;
        uniformsCS.reprojectionBlend = reprojectionBlend;
        uniformsCS.enableTemporalBlend = (enableTemporalBlend && historyValid) ? 1 : 0;
        uniformsCS.enableFullReproject = (enableFullReproject && historyValid) ? 1 : 0;

        uboCS.Write(uniformsCS);

        m_skyShader->Bind("uboCS", uboCS);

        m_skyShader->Bind("perlinNoiseTex", skyComponent->GetPerlinWorleyNoise()->GetTextureView());
        m_skyShader->Bind("perlinNoiseSampler", *linearRepeatedSampler);
        m_skyShader->Bind("weatherTex", skyComponent->GetWeather()->GetTextureView());
        m_skyShader->Bind("weatherSampler", *linearRepeatedSampler);

        m_skyShader->Bind("outFragColor", skyColorImageBuffer.image->GetTextureView());
        m_skyShader->Bind("outBloom", skyBloomImageBuffer.image->GetTextureView());
        m_skyShader->Bind("outAlphaness", skyAlphanessImageBuffer.image->GetTextureView());
        m_skyShader->Bind("outCloudDistance", skyCloudDistanceImageBuffer.image->GetTextureView());

        // Bind previous frame history for reprojection
        m_skyShader->Bind("prevColorTex", m_skyHistoryColorImageBuffer[viewIndex].image->GetTextureView());
        m_skyShader->Bind("prevColorSampler", *m_samplers[SamplerType::LINEAR]);
        m_skyShader->Bind("prevDepthTex", m_skyHistoryDepthImageBuffer[viewIndex].image->GetTextureView());
        m_skyShader->Bind("prevDepthSampler", *m_samplers[SamplerType::NEAREST]);

        const GfxBindGroup descriptorSetCompute = m_skyShader->UpdateNextBindGroup();

        GfxComputePassEncoder computePassEncoder{};
        GfxComputePassBeginDescriptor computePassDesc{};
        gfxCommandEncoderBeginComputePass(renderContext.commandEncoder, &computePassDesc, &computePassEncoder);
        gfxComputePassEncoderSetPipeline(computePassEncoder, *m_skyPipeline);
        gfxComputePassEncoderSetBindGroup(computePassEncoder, 0, descriptorSetCompute, nullptr, 0);

        const uint32_t dispatchX = prev::util::math::DispatchSize(extent.width, 16);
        const uint32_t dispatchY = prev::util::math::DispatchSize(extent.height, 16);
        gfxComputePassEncoderDispatch(computePassEncoder, dispatchX, dispatchY, 1);
        gfxComputePassEncoderEnd(computePassEncoder);

        // Copy current cloud output to history buffers for next frame's reprojection.
        // This keeps history coherent and avoids stale half-frames.
        if (enableCheckerboard || enableTemporalBlend || enableFullReproject) {
            skyColorImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_TRANSFER_SRC, renderContext.commandEncoder);
            skyCloudDistanceImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_TRANSFER_SRC, renderContext.commandEncoder);

            m_skyHistoryColorImageBuffer[viewIndex].image->UpdateLayout(GFX_TEXTURE_LAYOUT_TRANSFER_DST, renderContext.commandEncoder);
            m_skyHistoryDepthImageBuffer[viewIndex].image->UpdateLayout(GFX_TEXTURE_LAYOUT_TRANSFER_DST, renderContext.commandEncoder);

            CopyImageBuffer(skyColorImageBuffer, m_skyHistoryColorImageBuffer[viewIndex], renderContext.commandEncoder);
            CopyImageBuffer(skyCloudDistanceImageBuffer, m_skyHistoryDepthImageBuffer[viewIndex], renderContext.commandEncoder);

            m_skyHistoryColorImageBuffer[viewIndex].image->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, renderContext.commandEncoder);
            m_skyHistoryDepthImageBuffer[viewIndex].image->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, renderContext.commandEncoder);
        }

        skyColorImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, renderContext.commandEncoder);
        skyBloomImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, renderContext.commandEncoder);
        skyAlphanessImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, renderContext.commandEncoder);
        skyCloudDistanceImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, renderContext.commandEncoder);

        // Store current VP matrix for next frame's reprojection
        m_prevViewProjectionMatrix[viewIndex] = projectionMatrix * renderContext.viewMatrices[viewIndex];

        // sky post process render
        skyPostProcessColorImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_GENERAL, renderContext.commandEncoder);

        auto& uboPostCS = m_uniformsPoolSkyPostProcessCS->Next();

        glm::vec4 lightPositionClipSpace = renderContext.projectionMatrices[viewIndex] * renderContext.viewMatrices[viewIndex] * glm::vec4(mainLightComponent->GetPosition(), 1.0f);
        glm::vec3 lightPositionNdc = lightPositionClipSpace / lightPositionClipSpace.w;
        glm::vec2 lightPositionO1 = lightPositionNdc * 0.5f + 0.5f;

        UniformsSkyPostProcessCS uniformsPostCS{};
        uniformsPostCS.resolution = glm::vec4(extent.width, extent.height, 0.0f, 0.0f);
        uniformsPostCS.lisghtPosition = glm::vec4(lightPositionO1, 0.0f, 1.0f);
        uniformsPostCS.inverseProjectionMatrix = glm::inverse(renderContext.projectionMatrices[viewIndex]);
        uniformsPostCS.inverseViewMatrix = glm::inverse(renderContext.viewMatrices[viewIndex]);
        uniformsPostCS.lightDirection = glm::vec4(-mainLightComponent->GetDirection(), 0.0f);
        uniformsPostCS.skyColorBottom = glm::vec4(skyComponent->GetBottomColor(), 1.0f);
        uniformsPostCS.skyColorTop = glm::vec4(skyComponent->GetTopColor(), 1.0f);
        uniformsPostCS.enableGodRays = 1;
        uniformsPostCS.enableBlur = 1;
        uniformsPostCS.lightDotCameraForward = glm::dot(glm::normalize(renderContext.cameraPositions[viewIndex] - mainLightComponent->GetPosition()), glm::normalize(prev::util::math::GetForwardVector(renderContext.viewMatrices[viewIndex])));
        uniformsPostCS.godRaySamples = 64;
        uniformsPostCS.enableCheckerboardResolve = enableCheckerboard;
        uniformsPostCS.frameCounter = m_frameCounter;

        uboPostCS.Write(uniformsPostCS);

        m_skyPostProcessShader->Bind("uboCS", uboPostCS);

        m_skyPostProcessShader->Bind("skyTex", skyColorImageBuffer.image->GetTextureView());
        m_skyPostProcessShader->Bind("skySampler", *skyColorImageSampler);
        m_skyPostProcessShader->Bind("bloomTex", skyBloomImageBuffer.image->GetTextureView());
        m_skyPostProcessShader->Bind("bloomSampler", *skyBloomImageSampler);

        m_skyPostProcessShader->Bind("outFragColor", skyPostProcessColorImageBuffer.image->GetTextureView());

        const GfxBindGroup descriptorSetComputePost = m_skyPostProcessShader->UpdateNextBindGroup();

        GfxComputePassEncoder computePassEncoderPost{};
        GfxComputePassBeginDescriptor computePassPostDesc{};
        gfxCommandEncoderBeginComputePass(renderContext.commandEncoder, &computePassPostDesc, &computePassEncoderPost);
        gfxComputePassEncoderSetPipeline(computePassEncoderPost, *m_skyPostProcessPipeline);
        gfxComputePassEncoderSetBindGroup(computePassEncoderPost, 0, descriptorSetComputePost, nullptr, 0);

        gfxComputePassEncoderDispatch(computePassEncoderPost, prev::util::math::DispatchSize(extent.width, 16), prev::util::math::DispatchSize(extent.height, 16), 1);
        gfxComputePassEncoderEnd(computePassEncoderPost);

        skyPostProcessColorImageBuffer.image->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, renderContext.commandEncoder);
    }

    m_frameCounter++;
}

void SkyRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_compositePipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void SkyRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_SKY_RENDER_COMPONENT })) {
        return;
    }

    const auto skyComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::sky::ISkyComponent>(node);
    if (!skyComponent->IsReady()) {
        return;
    }

    for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
        auto& skyCloudDistanceImageBuffer{ m_skyCloudDistanceImageBuffer[viewIndex] };
        auto& skyPostProcessColorImageBuffer{ m_skyPostProcessColorImageBuffer[viewIndex] };

        auto& skyCloudDistanceImageSampler{ m_samplers[skyCloudDistanceImageBuffer.samplerType] };
        auto& skyPostProcessImageSampler{ m_samplers[skyPostProcessColorImageBuffer.samplerType] };
#ifdef ENABLE_XR
        const auto colorTexKey{ "colorTex[" + std::to_string(viewIndex) + "]" };
        const auto depthTexKey{ "depthTex[" + std::to_string(viewIndex) + "]" };
#else
        const auto colorTexKey{ "colorTex" };
        const auto depthTexKey{ "depthTex" };
#endif
        m_compositeShader->Bind(colorTexKey, skyPostProcessColorImageBuffer.image->GetTextureView());
        m_compositeShader->Bind("colorSampler", *skyPostProcessImageSampler);
        m_compositeShader->Bind(depthTexKey, skyCloudDistanceImageBuffer.image->GetTextureView());
        m_compositeShader->Bind("depthSampler", *skyCloudDistanceImageSampler);
    }

    const GfxBindGroup descriptorSet = m_compositeShader->UpdateNextBindGroup();
    const uint64_t vertexOffset = 0;
    const uint64_t vertexRange = skyComponent->GetModel()->GetVertexBuffer()->GetSize() - vertexOffset;
    gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *skyComponent->GetModel()->GetVertexBuffer(), vertexOffset, vertexRange);
    gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *skyComponent->GetModel()->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, skyComponent->GetModel()->GetIndexBuffer()->GetSize());
    gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

    gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, skyComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
}

void SkyRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SkyRenderer::EndFrame(const NormalRenderContext& renderContext)
{
    m_skyShader->EndFrame();
    m_skyPostProcessShader->EndFrame();
    m_compositeShader->EndFrame();
    m_uniformsPoolSkyCS->EndFrame();
    m_uniformsPoolSkyPostProcessCS->EndFrame();
}

void SkyRenderer::ShutDown()
{
    for (auto& sampler : m_samplers) {
        sampler.reset();
    }

    for (uint32_t viewIndex = 0; viewIndex < MAX_VIEW_COUNT; ++viewIndex) {
        m_skyHistoryDepthImageBuffer[viewIndex] = {};
        m_skyHistoryColorImageBuffer[viewIndex] = {};
        m_skyPostProcessColorImageBuffer[viewIndex] = {};
        m_skyCloudDistanceImageBuffer[viewIndex] = {};
        m_skyAlphanessImageBuffer[viewIndex] = {};
        m_skyBloomImageBuffer[viewIndex] = {};
        m_skyColorImageBuffer[viewIndex] = {};
    }

    m_uniformsPoolSkyPostProcessCS.reset();
    m_uniformsPoolSkyCS.reset();

    m_compositePipeline.reset();
    m_compositeShader.reset();

    m_skyPostProcessPipeline.reset();
    m_skyPostProcessShader.reset();

    m_skyPipeline.reset();
    m_skyShader.reset();
}

void SkyRenderer::UpdateImageBufferExtents(GfxCommandEncoder commandEncoder, const GfxExtent2D& extent, const GfxFormat format, ImageBufferData& imageBuffer, GfxTextureUsageFlags usageFlags)
{
    if (imageBuffer.image == nullptr || imageBuffer.image->GetExtent().width != extent.width || imageBuffer.image->GetExtent().height != extent.height) {
        // Record the layout transition into the frame encoder (called from BeginFrame, before any pass)
        // so a resize doesn't stall on an immediate submit+wait; ready when this frame's encoder submits.
        imageBuffer.image = prev::render::buffer::ImageBufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                                .SetExtent({ extent.width, extent.height, 1 })
                                .SetFormat(format)
                                .SetType(GFX_TEXTURE_TYPE_2D)
                                .SetUsageFlags(usageFlags)
                                .SetLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY)
                                .Build(commandEncoder);

        // TODO: gfx API doesn't expose format feature queries - assume linear filtering is available
        imageBuffer.samplerType = (format == DEPTH_FORMAT) ? SamplerType::NEAREST : SamplerType::LINEAR;
    }
}

void SkyRenderer::CopyImageBuffer(const ImageBufferData& src, const ImageBufferData& dst, GfxCommandEncoder commandEncoder)
{
    GfxCopyTextureToTextureDescriptor copyDesc{};
    copyDesc.source = src.image->GetTexture();
    copyDesc.sourceOrigin = { 0, 0, 0 };
    copyDesc.sourceMipLevel = 0;
    copyDesc.sourceArrayLayer = 0;
    copyDesc.sourceFinalLayout = GFX_TEXTURE_LAYOUT_TRANSFER_SRC;
    copyDesc.destination = dst.image->GetTexture();
    copyDesc.destinationOrigin = { 0, 0, 0 };
    copyDesc.destinationMipLevel = 0;
    copyDesc.destinationArrayLayer = 0;
    copyDesc.destinationFinalLayout = GFX_TEXTURE_LAYOUT_TRANSFER_DST;
    copyDesc.extent = src.image->GetExtent();
    gfxCommandEncoderCopyTextureToTexture(commandEncoder, &copyDesc);
}
} // namespace prev_test::render::renderer::sky
