#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <ThreadPool.h>

#include "General.h"

struct ShadowsRenderContextUserData : DefaultRenderContextUserData {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const uint32_t cascadeIndex;

    const Frustum frustum;

    ShadowsRenderContextUserData(const glm::mat4& vm, const glm::mat4& pm, const uint32_t index, const Frustum& frst)
        : viewMatrix(vm)
        , projectionMatrix(pm)
        , cascadeIndex(index)
        , frustum(frst)
    {
    }
};

struct NormalRenderContextUserData : DefaultRenderContextUserData {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const glm::vec3 cameraPosition;

    const glm::vec4 clipPlane;

    const VkExtent2D extent;

    const glm::vec2 nearFarClippingPlane;

    const Frustum frustum;

    NormalRenderContextUserData(const glm::mat4& vm, const glm::mat4& pm, const glm::vec3& camPos, const glm::vec4& cp, const VkExtent2D& ext, const glm::vec2& nearFar, const Frustum& frst)
        : viewMatrix(vm)
        , projectionMatrix(pm)
        , cameraPosition(camPos)
        , clipPlane(cp)
        , extent(ext)
        , nearFarClippingPlane(nearFar)
        , frustum(frst)
    {
    }
};

class DefaultShadowsRenderer : public IRenderer<ShadowsRenderContextUserData> {
private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<Uniforms> > m_uniformsPool;

public:
    DefaultShadowsRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~DefaultShadowsRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<DefaultShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/default_shadows_vert.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Default Shadows Shader created\n");

        m_pipeline = std::make_unique<DefaultShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Default Shadows Pipeline created\n");

        m_uniformsPool = std::make_unique<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void PreRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void BeforeRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
            }

            auto renderComponent = ComponentRepository<IRenderComponent>::Instance().Get(node->GetId());
            if (renderComponent->CastsShadows() && visible) {
                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                uniforms.projectionMatrix = shadowsRenderContext.projectionMatrix;
                uniforms.viewMatrix = shadowsRenderContext.viewMatrix;
                uniforms.modelMatrix = node->GetWorldTransformScaled();
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_pipeline->ShutDown();

        m_shader->ShutDown();
    }
};

class NormalMappedShadowsRenderer : public IRenderer<ShadowsRenderContextUserData> {
private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<Uniforms> > m_uniformsPool;

public:
    NormalMappedShadowsRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~NormalMappedShadowsRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<NormalMappedShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/normal_mapped_shadows_vert.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("NormalMapped Shadows Shader created\n");

        m_pipeline = std::make_unique<NormalMappedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("NormalMapped Shadows Pipeline created\n");

        m_uniformsPool = std::make_unique<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void PreRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void BeforeRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_NORMAL_MAPPED_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
            }

            auto renderComponent = ComponentRepository<IRenderComponent>::Instance().Get(node->GetId());
            if (renderComponent->CastsShadows() && visible) {
                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                uniforms.projectionMatrix = shadowsRenderContext.projectionMatrix;
                uniforms.viewMatrix = shadowsRenderContext.viewMatrix;
                uniforms.modelMatrix = node->GetWorldTransformScaled();
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_pipeline->ShutDown();

        m_shader->ShutDown();
    }
};

class TerrainShadowsRenderer : public IRenderer<ShadowsRenderContextUserData> {
private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
    };

private:
    const uint32_t m_descriptorCount{ 3000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<Uniforms> > m_uniformsPool;

public:
    TerrainShadowsRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~TerrainShadowsRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<TerrainShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/terrain_shadows_vert.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Terrain Shadows Shader created\n");

        m_pipeline = std::make_unique<TerrainShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Terrain Shadows Pipeline created\n");

        m_uniformsPool = std::make_unique<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void PreRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
            }

            if (visible) {
                const auto terrainComponent = ComponentRepository<ITerrainComponenet>::Instance().Get(node->GetId());
                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                uniforms.projectionMatrix = shadowsRenderContext.projectionMatrix;
                uniforms.viewMatrix = shadowsRenderContext.viewMatrix;
                uniforms.modelMatrix = node->GetWorldTransformScaled();
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *terrainComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_pipeline->ShutDown();

        m_shader->ShutDown();
    }
};

class TerrainNormalMappedShadowsRenderer : public IRenderer<ShadowsRenderContextUserData> {
private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
    };

private:
    const uint32_t m_descriptorCount{ 3000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<Uniforms> > m_uniformsPool;

public:
    TerrainNormalMappedShadowsRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~TerrainNormalMappedShadowsRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<TerrainNormalMappedShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/terrain_normal_mapped_shadows_vert.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Terrain Normal Mapped Shadows Shader created\n");

        m_pipeline = std::make_unique<TerrainNormalMappedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Terrain Normal Mapped Shadows Pipeline created\n");

        m_uniformsPool = std::make_unique<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void PreRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
            }

            if (visible) {
                const auto terrainComponent = ComponentRepository<ITerrainComponenet>::Instance().Get(node->GetId());
                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                uniforms.projectionMatrix = shadowsRenderContext.projectionMatrix;
                uniforms.viewMatrix = shadowsRenderContext.viewMatrix;
                uniforms.modelMatrix = node->GetWorldTransformScaled();
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *terrainComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_pipeline->ShutDown();

        m_shader->ShutDown();
    }
};

class AnimationShadowsRenderer : public IRenderer<ShadowsRenderContextUserData> {
private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
        alignas(16) glm::mat4 bones[MAX_BONES_COUNT];
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<Uniforms> > m_uniformsPool;

public:
    AnimationShadowsRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~AnimationShadowsRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<AnimatedShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/animation_shadows_vert.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Animation Shadows Shader created\n");

        m_pipeline = std::make_unique<AnimatedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Animation Shadows Pipeline created\n");

        m_uniformsPool = std::make_unique<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void PreRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
            }

            auto renderComponent = ComponentRepository<IAnimationRenderComponent>::Instance().Get(node->GetId());
            if (renderComponent->CastsShadows() && visible) {
                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                const auto& bones = renderComponent->GetAnimation()->GetBoneTransforms();
                for (size_t i = 0; i < bones.size(); i++) {
                    uniforms.bones[i] = bones[i];
                }
                uniforms.projectionMatrix = shadowsRenderContext.projectionMatrix;
                uniforms.viewMatrix = shadowsRenderContext.viewMatrix;
                uniforms.modelMatrix = node->GetWorldTransformScaled();
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_pipeline->ShutDown();

        m_shader->ShutDown();
    }
};

class AnimationNormalMappedShadowsRenderer : public IRenderer<ShadowsRenderContextUserData> {
private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
        alignas(16) glm::mat4 bones[MAX_BONES_COUNT];
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<Uniforms> > m_uniformsPool;

public:
    AnimationNormalMappedShadowsRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~AnimationNormalMappedShadowsRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<AnimatedNormalMappedShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/animation_normal_mapped_shadows_vert.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Animation Normal Mapped Shadows Shader created\n");

        m_pipeline = std::make_unique<AnimatedNormalMappedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Animation Normal Mapped Shadows Pipeline created\n");

        m_uniformsPool = std::make_unique<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void PreRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderUserData.frustum);
            }

            auto renderComponent = ComponentRepository<IAnimationRenderComponent>::Instance().Get(node->GetId());

            if (renderComponent->CastsShadows() && visible) {
                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                const auto& bones = renderComponent->GetAnimation()->GetBoneTransforms();
                for (size_t i = 0; i < bones.size(); i++) {
                    uniforms.bones[i] = bones[i];
                }
                uniforms.projectionMatrix = shadowsRenderUserData.projectionMatrix;
                uniforms.viewMatrix = shadowsRenderUserData.viewMatrix;
                uniforms.modelMatrix = node->GetWorldTransformScaled();
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_pipeline->ShutDown();

        m_shader->ShutDown();
    }
};

class ShadowMapDebugRenderer : public IRenderer<DefaultRenderContextUserData> {
private:
    struct PushConstantBlock {
        uint32_t imageIndex;
        float nearClippingPlane;
        float farClippingPlane;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    EventHandler<ShadowMapDebugRenderer, KeyEvent> m_keyEvent{ *this };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

private:
    std::unique_ptr<IModel> m_quadModel;

    int32_t m_cascadeIndex = 0;

public:
    ShadowMapDebugRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~ShadowMapDebugRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<ShadowMapDebugShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/shadow_map_debug_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/shadow_map_debug_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("ShadowMapDebug Shader created\n");

        m_pipeline = std::make_unique<ShadowMapDebugPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("ShadowMapDebug Pipeline created\n");

        // create quad model
        auto quadMesh = std::make_unique<QuadMesh>();

        auto vertexBuffer = std::make_unique<VBO>(*allocator);
        vertexBuffer->Data(quadMesh->GetVertexData(), quadMesh->GerVerticesCount(), quadMesh->GetVertexLayout().GetStride());

        auto indexBuffer = std::make_unique<IBO>(*allocator);
        indexBuffer->Data(quadMesh->GerIndices().data(), static_cast<uint32_t>(quadMesh->GerIndices().size()));

        m_quadModel = std::make_unique<Model>(std::move(quadMesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    void BeforeRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D renderRect{};
        renderRect.extent.width = renderContext.fullExtent.width / 2;
        renderRect.extent.height = renderContext.fullExtent.height / 2;
        renderRect.offset.x = 0;
        renderRect.offset.y = 0;

        VkRect2D scissor{};
        scissor.extent.width = renderContext.fullExtent.width;
        scissor.extent.height = renderContext.fullExtent.height;
        scissor.offset.x = 0;
        scissor.offset.y = 0;

        VkViewport viewport{};
        viewport.width = static_cast<float>(renderContext.fullExtent.width);
        viewport.height = static_cast<float>(renderContext.fullExtent.height);
        viewport.x = -static_cast<float>(renderContext.fullExtent.width / 2.0f);
        viewport.y = -static_cast<float>(renderContext.fullExtent.height / 2.0f);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    // make a node with quad model & shadowMap texture ???
    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto& cascade = shadows->GetCascade(static_cast<uint32_t>(m_cascadeIndex));
        PushConstantBlock pushConstBlock{ static_cast<uint32_t>(m_cascadeIndex), -cascade.startSplitDepth, -cascade.endSplitDepth };
        vkCmdPushConstants(renderContext.commandBuffer, m_pipeline->GetLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantBlock), &pushConstBlock);

        m_shader->Bind("depthSampler", shadows->GetImageBuffer()->GetImageView(), shadows->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

        VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();

        VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    void PostRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_O) {
                m_cascadeIndex = (m_cascadeIndex - 1) < 0 ? ShadowsComponent::CASCADES_COUNT - 1 : m_cascadeIndex - 1;
                std::cout << "New Cascade Index = " << m_cascadeIndex << std::endl;
            } else if (keyEvent.keyCode == KeyCode::KEY_P) {
                m_cascadeIndex = (m_cascadeIndex + 1) % ShadowsComponent::CASCADES_COUNT;
                std::cout << "New Cascade Index = " << m_cascadeIndex << std::endl;
            }
        }
    }
};

class TextureDebugRenderer : public IRenderer<DefaultRenderContextUserData> {
private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

private:
    std::unique_ptr<IModel> m_quadModel;

public:
    TextureDebugRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~TextureDebugRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<TextureDebugShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/texture_debug_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/texture_debug_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Texture Debug Shader created\n");

        m_pipeline = std::make_unique<TextureDebugPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Texture Debug Pipeline created\n");

        // create quad model
        auto quadMesh = std::make_unique<QuadMesh>();

        auto vertexBuffer = std::make_unique<VBO>(*allocator);
        vertexBuffer->Data(quadMesh->GetVertexData(), quadMesh->GerVerticesCount(), quadMesh->GetVertexLayout().GetStride());

        auto indexBuffer = std::make_unique<IBO>(*allocator);
        indexBuffer->Data(quadMesh->GerIndices().data(), static_cast<uint32_t>(quadMesh->GerIndices().size()));

        m_quadModel = std::make_unique<Model>(std::move(quadMesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    void BeforeRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D renderRect{};
        renderRect.extent.width = renderContext.fullExtent.width / 2;
        renderRect.extent.height = renderContext.fullExtent.height / 2;
        renderRect.offset.x = 0;
        renderRect.offset.y = 0;

        VkRect2D scissor{};
        scissor.extent.width = renderContext.fullExtent.width;
        scissor.extent.height = renderContext.fullExtent.height;
        scissor.offset.x = 0;
        scissor.offset.y = 0;

        VkViewport viewport{};
        viewport.width = static_cast<float>(renderContext.fullExtent.width);
        viewport.height = static_cast<float>(renderContext.fullExtent.height);
        viewport.x = -static_cast<float>(renderContext.fullExtent.width / 2.0f);
        viewport.y = -static_cast<float>(renderContext.fullExtent.height / 2.0f);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        const auto component = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFLECTION_RENDER_COMPONENT });

        m_shader->Bind("imageSampler", component->GetColorImageBuffer()->GetImageView(), component->GetColorImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();

        VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    void PostRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

#ifdef RENDER_BOUNDING_VOLUMES
class BoundingVolumeDebugRenderer : public IRenderer<NormalRenderContextUserData> {
private:
    struct alignas(16) UniformsVS
    {
        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) glm::vec4 color;

        alignas(16) glm::vec4 selectedColor;

        alignas(16) uint32_t selected;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;
   
public:
    BoundingVolumeDebugRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~BoundingVolumeDebugRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<BoundingVolumeDebugShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/bounding_volume_debug_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/bounding_volume_debug_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Bounding Volume Debug Shader created\n");

        m_pipeline = std::make_unique<BoundingVolumeDebugPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Bounding Volume Debug Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_BOUNDING_VOLUME_COMPONENT })) {
            const auto boundingVolumeComponent = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId());

            auto uboVS = m_uniformsPoolVS->GetNext();

            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
            uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
            uniformsVS.modelMatrix = glm::mat4(1.0f);

            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();

            UniformsFS uniformsFS{};
            uniformsFS.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
            uniformsFS.selectedColor = SELECTED_COLOR;
            uniformsFS.selected = false;
            
            uboFS->Update(&uniformsFS);

            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            VkBuffer vertexBuffers[] = { *boundingVolumeComponent->GetModel()->GetVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *boundingVolumeComponent->GetModel()->GetIndexBuffer(), 0, boundingVolumeComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, boundingVolumeComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};
#endif

class DefaultRenderer : public IRenderer<NormalRenderContextUserData> {
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
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    DefaultRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~DefaultRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<DefaultShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/default_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/default_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Default Shader created\n");

        m_pipeline = std::make_unique<DefaultPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Default Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
            }

            if (visible) {
                const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
                const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
                const auto lightComponents = GraphTraversalHelper::GetNodeComponents<SceneNodeFlags, ILightComponent>({ TAG_LIGHT });

                const auto nodeRenderComponent = ComponentRepository<IRenderComponent>::Instance().Get(node->GetId());

                auto uboVS = m_uniformsPoolVS->GetNext();

                UniformsVS uniformsVS{};
                uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
                uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
                uniformsVS.modelMatrix = node->GetWorldTransformScaled();
                uniformsVS.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
                uniformsVS.textureNumberOfRows = nodeRenderComponent->GetMaterial()->GetAtlasNumberOfRows();
                uniformsVS.textureOffset = glm::vec4(nodeRenderComponent->GetMaterial()->GetTextureOffset(), 0.0f, 0.0f);
                uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsVS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;
                uniformsVS.useFakeLightning = nodeRenderComponent->GetMaterial()->UsesFakeLightning();
                uniformsVS.density = FOG_DENSITY;
                uniformsVS.gradient = FOG_GRADIENT;
                uniformsVS.clipPlane = renderContextUserData.clipPlane;

                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();

                UniformsFS uniformsFS{};
                // shadows
                for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                    auto& cascade = shadowsComponent->GetCascade(i);
                    uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
                }
                uniformsFS.shadows.enabled = SHADOWS_ENABLED;

                // lightning
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsFS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsFS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;

                // material
                uniformsFS.material = MaterialUniform(nodeRenderComponent->GetMaterial()->GetShineDamper(), nodeRenderComponent->GetMaterial()->GetReflectivity());

                // common
                uniformsFS.fogColor = FOG_COLOR;
                uniformsFS.selectedColor = SELECTED_COLOR;
                uniformsFS.selected = false;
                uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();

                uboFS->Update(&uniformsFS);

                m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
                m_shader->Bind("textureSampler", *nodeRenderComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class NormalMappedRenderer : public IRenderer<NormalRenderContextUserData> {
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
        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;

        alignas(16) glm::mat4 normalMatrix;

        alignas(16) glm::vec4 clipPlane;

        alignas(16) glm::vec4 cameraPosition;

        alignas(16) LightningUniform lightning;

        alignas(16) glm::vec4 textureOffset;

        alignas(16) uint32_t textureNumberOfRows;
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
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    NormalMappedRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~NormalMappedRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<NormalMappedShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/normal_mapped_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/normal_mapped_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("NormalMapped Shader created\n");

        m_pipeline = std::make_unique<NormalMappedPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("NormalMapped Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_NORMAL_MAPPED_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
            }

            if (visible) {
                const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
                const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
                const auto lightComponents = GraphTraversalHelper::GetNodeComponents<SceneNodeFlags, ILightComponent>({ TAG_LIGHT });

                const auto nodeRenderComponent = ComponentRepository<IRenderComponent>::Instance().Get(node->GetId());

                auto uboVS = m_uniformsPoolVS->GetNext();

                UniformsVS uniformsVS{};
                uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
                uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
                uniformsVS.modelMatrix = node->GetWorldTransformScaled();
                uniformsVS.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
                uniformsVS.textureNumberOfRows = nodeRenderComponent->GetMaterial()->GetAtlasNumberOfRows();
                uniformsVS.textureOffset = glm::vec4(nodeRenderComponent->GetMaterial()->GetTextureOffset(), 0.0f, 0.0f);
                uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsVS.lightning.lights[i] = LightUniform(renderContextUserData.viewMatrix * glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsVS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;
                uniformsVS.density = FOG_DENSITY;
                uniformsVS.gradient = FOG_GRADIENT;
                uniformsVS.clipPlane = renderContextUserData.clipPlane;

                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();

                UniformsFS uniformsFS{};
                // shadows
                for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                    auto& cascade = shadowsComponent->GetCascade(i);
                    uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
                }
                uniformsFS.shadows.enabled = SHADOWS_ENABLED;

                // lightning
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsFS.lightning.lights[i] = LightUniform(renderContextUserData.viewMatrix * glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsFS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;

                // material
                uniformsFS.material = MaterialUniform(nodeRenderComponent->GetMaterial()->GetShineDamper(), nodeRenderComponent->GetMaterial()->GetReflectivity());

                // common
                uniformsFS.fogColor = FOG_COLOR;
                uniformsFS.selectedColor = SELECTED_COLOR;
                uniformsFS.selected = false;
                uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();

                uboFS->Update(&uniformsFS);

                m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
                m_shader->Bind("textureSampler", *nodeRenderComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("normalSampler", *nodeRenderComponent->GetMaterial()->GetNormalmageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class AnimationRenderer : public IRenderer<NormalRenderContextUserData> {
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
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    AnimationRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~AnimationRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<AnimationShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/animation_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/animation_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Animation Shader created\n");

        m_pipeline = std::make_unique<AnimationPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Animation Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
            }

            if (visible) {
                const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
                const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
                const auto lightComponents = GraphTraversalHelper::GetNodeComponents<SceneNodeFlags, ILightComponent>({ TAG_LIGHT });

                const auto nodeRenderComponent = ComponentRepository<IAnimationRenderComponent>::Instance().Get(node->GetId());

                auto uboVS = m_uniformsPoolVS->GetNext();

                UniformsVS uniformsVS{};
                const auto& bones = nodeRenderComponent->GetAnimation()->GetBoneTransforms();
                for (size_t i = 0; i < bones.size(); i++) {
                    uniformsVS.bones[i] = bones[i];
                }
                uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
                uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
                uniformsVS.modelMatrix = node->GetWorldTransformScaled();
                uniformsVS.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
                uniformsVS.textureNumberOfRows = nodeRenderComponent->GetMaterial()->GetAtlasNumberOfRows();
                uniformsVS.textureOffset = glm::vec4(nodeRenderComponent->GetMaterial()->GetTextureOffset(), 0.0f, 0.0f);
                uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsVS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;
                uniformsVS.useFakeLightning = nodeRenderComponent->GetMaterial()->UsesFakeLightning();
                uniformsVS.density = FOG_DENSITY;
                uniformsVS.gradient = FOG_GRADIENT;
                uniformsVS.clipPlane = renderContextUserData.clipPlane;

                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();

                UniformsFS uniformsFS{};
                // shadows
                for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                    auto& cascade = shadowsComponent->GetCascade(i);
                    uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
                }
                uniformsFS.shadows.enabled = SHADOWS_ENABLED;

                // lightning
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsFS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsFS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;

                // material
                uniformsFS.material = MaterialUniform(nodeRenderComponent->GetMaterial()->GetShineDamper(), nodeRenderComponent->GetMaterial()->GetReflectivity());

                // common
                uniformsFS.fogColor = FOG_COLOR;
                uniformsFS.selectedColor = SELECTED_COLOR;
                uniformsFS.selected = false;
                uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();

                uboFS->Update(&uniformsFS);

                m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
                m_shader->Bind("textureSampler", *nodeRenderComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class AnimationNormalMappedRenderer : public IRenderer<NormalRenderContextUserData> {
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
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    AnimationNormalMappedRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~AnimationNormalMappedRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<AnimationNormalMappedShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/animation_normal_mapped_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/animation_normal_mapped_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Animation Normal Mapped Shader created\n");

        m_pipeline = std::make_unique<AnimationNormalMappedPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Animation Normal Mapped Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT })) {
            
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
            }

            if (visible) {
                const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
                const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
                const auto lightComponents = GraphTraversalHelper::GetNodeComponents<SceneNodeFlags, ILightComponent>({ TAG_LIGHT });

                const auto nodeRenderComponent = ComponentRepository<IAnimationRenderComponent>::Instance().Get(node->GetId());

                auto uboVS = m_uniformsPoolVS->GetNext();

                UniformsVS uniformsVS{};
                const auto& bones = nodeRenderComponent->GetAnimation()->GetBoneTransforms();
                for (size_t i = 0; i < bones.size(); i++) {
                    uniformsVS.bones[i] = bones[i];
                }
                uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
                uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
                uniformsVS.modelMatrix = node->GetWorldTransformScaled();
                uniformsVS.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
                uniformsVS.textureNumberOfRows = nodeRenderComponent->GetMaterial()->GetAtlasNumberOfRows();
                uniformsVS.textureOffset = glm::vec4(nodeRenderComponent->GetMaterial()->GetTextureOffset(), 0.0f, 0.0f);
                uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsVS.lightning.lights[i] = LightUniform(renderContextUserData.viewMatrix * glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsVS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;
                uniformsVS.density = FOG_DENSITY;
                uniformsVS.gradient = FOG_GRADIENT;
                uniformsVS.clipPlane = renderContextUserData.clipPlane;

                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();

                UniformsFS uniformsFS{};
                // shadows
                for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                    auto& cascade = shadowsComponent->GetCascade(i);
                    uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
                }
                uniformsFS.shadows.enabled = SHADOWS_ENABLED;

                // lightning
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsFS.lightning.lights[i] = LightUniform(renderContextUserData.viewMatrix * glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsFS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;

                // material
                uniformsFS.material = MaterialUniform(nodeRenderComponent->GetMaterial()->GetShineDamper(), nodeRenderComponent->GetMaterial()->GetReflectivity());

                // common
                uniformsFS.fogColor = FOG_COLOR;
                uniformsFS.selectedColor = SELECTED_COLOR;
                uniformsFS.selected = false;
                uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();

                uboFS->Update(&uniformsFS);

                m_shader->Bind("textureSampler", *nodeRenderComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("normalSampler", *nodeRenderComponent->GetMaterial()->GetNormalmageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class TerrainRenderer : public IRenderer<NormalRenderContextUserData> {
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
        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;

        alignas(16) glm::mat4 normalMatrix;

        alignas(16) glm::vec4 clipPlane;

        alignas(16) glm::vec4 cameraPosition;

        alignas(16) LightningUniform lightning;

        alignas(16) float density;
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
        float minHeight;
        float maxHeight;

        alignas(16) glm::vec4 heightSteps[4];
        alignas(16) float heightTtransitionRange;
    };

private:
    const uint32_t m_descriptorCount{ 3000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    TerrainRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~TerrainRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<TerrainShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/terrain_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/terrain_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Terrain Shader created\n");

        m_pipeline = std::make_unique<TerrainPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Terrain Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
            }

            if (visible) {
                const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
                const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
                const auto lightComponents = GraphTraversalHelper::GetNodeComponents<SceneNodeFlags, ILightComponent>({ TAG_LIGHT });

                const auto terrainComponent = ComponentRepository<ITerrainComponenet>::Instance().Get(node->GetId());

                auto uboVS = m_uniformsPoolVS->GetNext();

                UniformsVS uniformsVS{};
                uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
                uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
                uniformsVS.modelMatrix = node->GetWorldTransformScaled();
                uniformsVS.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
                uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsVS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;
                uniformsVS.density = FOG_DENSITY;
                uniformsVS.gradient = FOG_GRADIENT;
                uniformsVS.clipPlane = renderContextUserData.clipPlane;

                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();

                UniformsFS uniformsFS{};
                // shadows
                for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                    const auto& cascade = shadowsComponent->GetCascade(i);
                    uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
                }
                uniformsFS.shadows.enabled = SHADOWS_ENABLED;

                // lightning
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsFS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsFS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;

                // material
                uniformsFS.material = MaterialUniform(terrainComponent->GetMaterials().at(0)->GetShineDamper(), terrainComponent->GetMaterials().at(0)->GetReflectivity());

                // common
                uniformsFS.fogColor = FOG_COLOR;
                uniformsFS.selectedColor = SELECTED_COLOR;
                uniformsFS.selected = false;
                uniformsFS.castedByShadows = true;
                uniformsFS.minHeight = terrainComponent->GetHeightMapInfo()->GetGlobalMinHeight();
                uniformsFS.maxHeight = terrainComponent->GetHeightMapInfo()->GetGlobalMaxHeight();
                for (uint32_t i = 0; i < terrainComponent->GetHeightSteps().size(); i++) {
                    uniformsFS.heightSteps[i] = glm::vec4(terrainComponent->GetHeightSteps().at(i));
                }
                uniformsFS.heightTtransitionRange = terrainComponent->GetTransitionRange();

                uboFS->Update(&uniformsFS);

                m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
                for (uint32_t i = 0; i < 4; i++) {
                    m_shader->Bind("textureSampler[" + std::to_string(i) + "]", *terrainComponent->GetMaterials().at(i)->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                }
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *terrainComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class TerrainNormalMappedRenderer : public IRenderer<NormalRenderContextUserData> {
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
        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;

        alignas(16) glm::mat4 normalMatrix;

        alignas(16) glm::vec4 clipPlane;

        alignas(16) glm::vec4 cameraPosition;

        alignas(16) LightningUniform lightning;

        alignas(16) float density;
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
        float minHeight;
        float maxHeight;

        alignas(16) glm::vec4 heightSteps[4];
        alignas(16) float heightTtransitionRange;
    };

private:
    const uint32_t m_descriptorCount{ 3000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    TerrainNormalMappedRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~TerrainNormalMappedRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<TerrainNormalMappedShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/terrain_normal_mapped_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/terrain_normal_mapped_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Terrain Normal Mapped Shader created\n");

        m_pipeline = std::make_unique<TerrainNormalMappedPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Terrain Normal Mapped Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
            }

            if (visible) {
                const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
                const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
                const auto lightComponents = GraphTraversalHelper::GetNodeComponents<SceneNodeFlags, ILightComponent>({ TAG_LIGHT });

                const auto terrainComponent = ComponentRepository<ITerrainComponenet>::Instance().Get(node->GetId());

                auto uboVS = m_uniformsPoolVS->GetNext();

                UniformsVS uniformsVS{};
                uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
                uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
                uniformsVS.modelMatrix = node->GetWorldTransformScaled();
                uniformsVS.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
                uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsVS.lightning.lights[i] = LightUniform(renderContextUserData.viewMatrix * glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsVS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;
                uniformsVS.density = FOG_DENSITY;
                uniformsVS.gradient = FOG_GRADIENT;
                uniformsVS.clipPlane = renderContextUserData.clipPlane;

                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();

                UniformsFS uniformsFS{};
                // shadows
                for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                    const auto& cascade = shadowsComponent->GetCascade(i);
                    uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
                }
                uniformsFS.shadows.enabled = SHADOWS_ENABLED;

                // lightning
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsFS.lightning.lights[i] = LightUniform(renderContextUserData.viewMatrix * glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsFS.lightning.ambientFactor = AMBIENT_LIGHT_INTENSITY;

                // material
                uniformsFS.material = MaterialUniform(terrainComponent->GetMaterials().at(0)->GetShineDamper(), terrainComponent->GetMaterials().at(0)->GetReflectivity());

                // common
                uniformsFS.fogColor = FOG_COLOR;
                uniformsFS.selectedColor = SELECTED_COLOR;
                uniformsFS.selected = false;
                uniformsFS.castedByShadows = true;
                uniformsFS.minHeight = terrainComponent->GetHeightMapInfo()->GetGlobalMinHeight();
                uniformsFS.maxHeight = terrainComponent->GetHeightMapInfo()->GetGlobalMaxHeight();
                for (uint32_t i = 0; i < terrainComponent->GetHeightSteps().size(); i++) {
                    uniformsFS.heightSteps[i] = glm::vec4(terrainComponent->GetHeightSteps().at(i));
                }
                uniformsFS.heightTtransitionRange = terrainComponent->GetTransitionRange();

                uboFS->Update(&uniformsFS);

                for (uint32_t i = 0; i < 4; i++) {
                    m_shader->Bind("textureSampler[" + std::to_string(i) + "]", *terrainComponent->GetMaterials().at(i)->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                    m_shader->Bind("normalSampler[" + std::to_string(i) + "]", *terrainComponent->GetMaterials().at(i)->GetNormalmageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                }
                m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *terrainComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class FontRenderer : public IRenderer<NormalRenderContextUserData> {
private:
    struct alignas(16) UniformsVS
    {
        alignas(16) glm::vec4 translation;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) glm::vec4 color;

        alignas(16) float width;

        alignas(16) float edge;

        alignas(16) float borderWidth;

        alignas(16) float borderEdge;

        alignas(16) uint32_t hasEffect;

        alignas(16) glm::vec4 outlineColor;

        alignas(16) glm::vec4 outlineOffset;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    FontRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~FontRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<FonttShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/font_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/font_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Fonts Shader created\n");

        m_pipeline = std::make_unique<FontPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Fonts Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_FONT_RENDER_COMPONENT })) {
            const auto nodeFontRenderComponent = ComponentRepository<IFontRenderComponent>::Instance().Get(node->GetId());
            for (const auto& renderableText : nodeFontRenderComponent->GetRenderableTexts()) {
                auto uboVS = m_uniformsPoolVS->GetNext();
                UniformsVS uniformsVS{};
                uniformsVS.translation = glm::vec4(renderableText.text->GetPosition(), 0.0f, 1.0f);
                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();
                UniformsFS uniformsFS{};
                uniformsFS.color = renderableText.text->GetColor();
                uniformsFS.width = renderableText.text->GetWidth();
                uniformsFS.edge = renderableText.text->GetEdge();
                uniformsFS.borderWidth = renderableText.text->GetBorderWidth();
                uniformsFS.borderEdge = renderableText.text->GetBorderEdge();
                uniformsFS.hasEffect = renderableText.text->HasEffect();
                uniformsFS.outlineColor = glm::vec4(renderableText.text->GetOutlineColor(), 1.0f);
                uniformsFS.outlineOffset = glm::vec4(renderableText.text->GetOutlineOffset(), 0.0f, 1.0f);
                uboFS->Update(&uniformsFS);

                m_shader->Bind("textureSampler", *nodeFontRenderComponent->GetFontMetadata()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *renderableText.model->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderableText.model->GetIndexBuffer(), 0, renderableText.model->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, renderableText.model->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class SkyBoxRenderer : public IRenderer<NormalRenderContextUserData> {
private:
    struct alignas(16) UniformsVS
    {
        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) glm::vec4 fogColor;

        alignas(16) float lowerLimit;

        alignas(16) float upperLimit;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    SkyBoxRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~SkyBoxRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<SkyBoxShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/skybox_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/skybox_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Skybox Shader created\n");

        m_pipeline = std::make_unique<SkyBoxPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Skybox Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_SKYBOX_RENDER_COMPONENT })) {
            const auto skyBoxComponent = ComponentRepository<ISkyBoxComponent>::Instance().Get(node->GetId());

            auto uboVS = m_uniformsPoolVS->GetNext();

            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
            uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
            uniformsVS.modelMatrix = node->GetWorldTransformScaled();

            // TODO should we use CLIP_PLANE here?

            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();

            UniformsFS uniformsFS{};
            uniformsFS.fogColor = FOG_COLOR;
            uniformsFS.lowerLimit = 0.0f;
            uniformsFS.upperLimit = 30.0f;
            uboFS->Update(&uniformsFS);

            m_shader->Bind("cubeMap1", *skyBoxComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            VkBuffer vertexBuffers[] = { *skyBoxComponent->GetModel()->GetVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *skyBoxComponent->GetModel()->GetIndexBuffer(), 0, skyBoxComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, skyBoxComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class WaterRenderer : public IRenderer<NormalRenderContextUserData> {
private:
    struct ShadowsCascadeUniform {
        glm::mat4 viewProjectionMatrix;

        glm::vec4 split;
    };

    struct ShadowsUniform {
        ShadowsCascadeUniform cascades[ShadowsComponent::CASCADES_COUNT];

        uint32_t enabled;
    };

    struct LightUniform {
        glm::vec4 position;

        glm::vec4 color;
    };

    struct alignas(16) UniformsVS
    {
        alignas(16) glm::mat4 modelMatrix;

        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;

        alignas(16) glm::vec4 cameraPosition;

        alignas(16) float density;

        alignas(16) float gradient;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) ShadowsUniform shadows;

        alignas(16) glm::vec4 fogColor;

        alignas(16) glm::vec4 waterColor;

        alignas(16) LightUniform light;

        alignas(16) glm::vec4 nearFarClippingPlane;

        alignas(16) float moveFactor;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    WaterRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~WaterRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<WaterShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/water_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/water_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Water Shader created\n");

        m_pipeline = std::make_unique<WaterPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Water Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_RENDER_COMPONENT })) {
            bool visible = true;
            if (ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
                visible = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
            }

            if (visible) {
                const auto waterComponent = ComponentRepository<IWaterComponent>::Instance().Get(node->GetId());
                const auto waterReflectionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFLECTION_RENDER_COMPONENT });
                const auto waterRefractionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFRACTION_RENDER_COMPONENT });

                const auto mainLightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
                const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

                auto uboVS = m_uniformsPoolVS->GetNext();

                UniformsVS uniformsVS{};
                uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
                uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
                uniformsVS.modelMatrix = node->GetWorldTransformScaled();
                uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
                uniformsVS.density = FOG_DENSITY;
                uniformsVS.gradient = FOG_GRADIENT;

                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();

                UniformsFS uniformsFS{};
                uniformsFS.fogColor = FOG_COLOR;
                uniformsFS.waterColor = glm::vec4(waterComponent->GetMaterial()->GetColor(), 1.0f);
                uniformsFS.light.color = glm::vec4(mainLightComponent->GetColor(), 1.0f);
                uniformsFS.light.position = glm::vec4(mainLightComponent->GetPosition(), 1.0f);
                uniformsFS.nearFarClippingPlane = glm::vec4(renderContextUserData.nearFarClippingPlane, 0.0f, 0.0f);
                uniformsFS.moveFactor = waterComponent->GetMoveFactor();
                // shadows
                for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
                    const auto& cascade = shadowsComponent->GetCascade(i);
                    uniformsFS.shadows.cascades[i].split = glm::vec4(cascade.endSplitDepth);
                    uniformsFS.shadows.cascades[i].viewProjectionMatrix = cascade.GetBiasedViewProjectionMatrix();
                }
                uniformsFS.shadows.enabled = SHADOWS_ENABLED;

                uboFS->Update(&uniformsFS);

                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);
                m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
                m_shader->Bind("reflectionTexture", waterReflectionComponent->GetColorImageBuffer()->GetImageView(), waterReflectionComponent->GetColorImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("refractionTexture", waterRefractionComponent->GetColorImageBuffer()->GetImageView(), waterRefractionComponent->GetColorImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("dudvMapTexture", waterComponent->GetMaterial()->GetImageBuffer()->GetImageView(), waterComponent->GetMaterial()->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("normalMapTexture", waterComponent->GetMaterial()->GetNormalmageBuffer()->GetImageView(), waterComponent->GetMaterial()->GetNormalmageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("depthMapTexture", waterRefractionComponent->GetDepthImageBuffer()->GetImageView(), waterRefractionComponent->GetDepthImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *waterComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *waterComponent->GetModel()->GetIndexBuffer(), 0, waterComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, waterComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

struct SunVisibilityEvent {
    float visibilityFactor;
};

class SunRenderer : public IRenderer<NormalRenderContextUserData> {
private:
    struct alignas(16) UniformsVS
    {
        alignas(16) glm::vec4 translation;

        alignas(16) glm::vec4 scale;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) glm::vec4 brightness;
    };

private:
    const uint32_t m_descriptorCount{ 50 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

private:
    uint64_t m_passedSamples{ 0 };

    uint64_t m_maxNumberOfSamples{ 0 };

    VkQueryPool m_queryPool{ nullptr };

public:
    SunRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~SunRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<FlareShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/sun_occlusion_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/sun_occlusion_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("Sun Shader created\n");

        m_pipeline = std::make_unique<FlarePipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Sun Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        VkQueryPoolCreateInfo queryPoolInfo = { VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
        queryPoolInfo.queryType = VK_QUERY_TYPE_OCCLUSION;
        queryPoolInfo.queryCount = 1;
        VKERRCHECK(vkCreateQueryPool(*device, &queryPoolInfo, nullptr, &m_queryPool));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        m_passedSamples = 0;
        vkCmdResetQueryPool(renderContext.commandBuffer, m_queryPool, 0, 1);
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_SUN_RENDER_COMPONENT })) {
            const auto sunComponent = ComponentRepository<ISunComponent>::Instance().Get(node->GetId());

            const float xScale = sunComponent->GetFlare()->GetScale();
            const float yScale = xScale * renderContext.fullExtent.width / renderContext.fullExtent.height;

            m_maxNumberOfSamples = static_cast<uint64_t>(powf(sunComponent->GetFlare()->GetScale() * renderContext.fullExtent.width, 2.0f));

            vkCmdBeginQuery(renderContext.commandBuffer, m_queryPool, 0, 0);

            auto uboVS = m_uniformsPoolVS->GetNext();
            UniformsVS uniformsVS{};
            uniformsVS.translation = glm::vec4(sunComponent->GetFlare()->GetScreenSpacePosition(), 0.0f, 1.0f);
            uniformsVS.scale = glm::vec4(xScale, yScale, 0.0f, 0.0f);
            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();
            UniformsFS uniformsFS{};
            uniformsFS.brightness = glm::vec4(1.0f);
            uboFS->Update(&uniformsFS);

            m_shader->Bind("textureSampler", *sunComponent->GetFlare()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            VkBuffer vertexBuffers[] = { *sunComponent->GetModel()->GetVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *sunComponent->GetModel()->GetIndexBuffer(), 0, sunComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, sunComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);

            vkCmdEndQuery(renderContext.commandBuffer, m_queryPool, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        auto device = DeviceProvider::Instance().GetDevice();

//#if defined(__ANDROID__)
        auto result = vkGetQueryPoolResults(*device, m_queryPool, 0, 1, sizeof(m_passedSamples), &m_passedSamples, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_PARTIAL_BIT);
//#else
//        auto result = vkGetQueryPoolResults(*device, m_queryPool, 0, 1, sizeof(m_passedSamples), &m_passedSamples, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
//#endif
        const float ratio = glm::clamp((static_cast<float>(m_passedSamples) / static_cast<float>(m_maxNumberOfSamples)), 0.0f, 1.0f) * 0.8f;
        //LOGI("Result: %s Passed samples: %lld Max: %lld Ratio: %f\n", VkResultStr(result),  m_passedSamples,  m_maxNumberOfSamples, ratio);
        EventChannel::Broadcast(SunVisibilityEvent{ ratio });
    }

    void ShutDown() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        vkDestroyQueryPool(*device, m_queryPool, nullptr);

        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class LensFlareRenderer : public IRenderer<NormalRenderContextUserData> {
private:
    EventHandler<LensFlareRenderer, SunVisibilityEvent> m_sunVisibilityEventHandler{ *this };

    float m_sunVisibilityFactor{ 0.0f };

private:
    struct alignas(16) UniformsVS
    {
        alignas(16) glm::vec4 translation;

        alignas(16) glm::vec4 scale;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) glm::vec4 brightness;
    };

private:
    const uint32_t m_descriptorCount{ 50 };

private:
    std::shared_ptr<RenderPass> m_renderPass;

private:
    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<IGraphicsPipeline> m_pipeline;

    std::unique_ptr<UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<UBOPool<UniformsFS> > m_uniformsPoolFS;

public:
    LensFlareRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_renderPass(renderPass)
    {
    }

    virtual ~LensFlareRenderer() = default;

public:
    void Init() override
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory;
        m_shader = shaderFactory.CreateShaderFromFiles<FlareShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/lens_flare_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/lens_flare_frag.spv") } });
        m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

        LOGI("LensFlare Shader created\n");

        m_pipeline = std::make_unique<FlarePipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("LensFlare Pipeline created\n");

        m_uniformsPoolVS = std::make_unique<UBOPool<UniformsVS> >(*allocator);
        m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        m_uniformsPoolFS = std::make_unique<UBOPool<UniformsFS> >(*allocator);
        m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_LENS_FLARE_RENDER_COMPONENT })) {
            const auto lensFlareComponent = ComponentRepository<ILensFlareComponent>::Instance().Get(node->GetId());

            for (const auto& lensFlare : lensFlareComponent->GetFlares()) {
                const float xScale = lensFlare->GetScale();
                const float yScale = xScale * renderContext.fullExtent.width / renderContext.fullExtent.height;

                auto uboVS = m_uniformsPoolVS->GetNext();
                UniformsVS uniformsVS{};
                uniformsVS.translation = glm::vec4(lensFlare->GetScreenSpacePosition(), 0.0f, 1.0f);
                uniformsVS.scale = glm::vec4(xScale, yScale, 0.0f, 0.0f);
                uboVS->Update(&uniformsVS);

                auto uboFS = m_uniformsPoolFS->GetNext();
                UniformsFS uniformsFS{};
                uniformsFS.brightness = glm::vec4(m_sunVisibilityFactor);
                uboFS->Update(&uniformsFS);

                m_shader->Bind("textureSampler", *lensFlare->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                VkBuffer vertexBuffers[] = { *lensFlareComponent->GetModel()->GetVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *lensFlareComponent->GetModel()->GetIndexBuffer(), 0, lensFlareComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, lensFlareComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }

    void operator()(const SunVisibilityEvent& evt)
    {
        m_sunVisibilityFactor = evt.visibilityFactor;
    }
};

class CommandBuffersGroup {
public:
    CommandBuffersGroup(const std::vector<std::vector<VkCommandPool> >& pools, const std::vector<std::vector<VkCommandBuffer> >& commandBuffers)
        : m_commandPoolGroups(pools)
        , m_commandBufferGroups(commandBuffers)
    {
    }

    ~CommandBuffersGroup() {
        auto device = DeviceProvider::Instance().GetDevice();       
       
        vkDeviceWaitIdle(*device);

        for (auto& poolGroup : m_commandPoolGroups) {
            for (auto& pool : poolGroup) {
                vkDestroyCommandPool(*device, pool, nullptr);
            }
        }
    }

public:
    const std::vector<VkCommandBuffer>& GetBuffersGroup(const uint32_t index) const
    {
        return m_commandBufferGroups.at(index);
    }

private:
    std::vector<std::vector<VkCommandPool> > m_commandPoolGroups{};

    std::vector<std::vector<VkCommandBuffer> > m_commandBufferGroups{};
};

class CommandBuffersGroupFactory {
public:
    std::unique_ptr<CommandBuffersGroup> CreateGroup(const Queue& queue, const uint32_t dim0Size, const uint32_t groupSize)
    {
        auto device = DeviceProvider::Instance().GetDevice();

        std::vector<std::vector<VkCommandPool> > commandPools{};
        std::vector<std::vector<VkCommandBuffer> > commandBuffers{};
        commandPools.reserve(dim0Size);
        commandBuffers.reserve(dim0Size);

        for (uint32_t dim0Index = 0; dim0Index < dim0Size; dim0Index++) {
            std::vector<VkCommandPool> pools{};
            std::vector<VkCommandBuffer> buffers{};
            for (uint32_t shadowRendererIndex = 0; shadowRendererIndex < groupSize; shadowRendererIndex++) {
                auto pool = queue.CreateCommandPool();
                pools.push_back(pool);
                buffers.push_back(VkUtils::CreateCommandBuffer(*device, pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY));
            }
            commandPools.push_back(pools);
            commandBuffers.push_back(buffers);
        }

        return std::make_unique<CommandBuffersGroup>(commandPools, commandBuffers);
    }
};

//#define PARALLEL_RENDERING

class MasterRenderer : public IRenderer<DefaultRenderContextUserData> {
public:
    MasterRenderer(const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<Swapchain>& swapchain)
        : m_defaultRenderPass(renderPass)
        , m_swapchain(swapchain)
    {
    }

    ~MasterRenderer() = default;

public:
    void Init() override
    {
        InitDefault();
        InitDebug();
        InitShadows();
        InitReflection();
        InitRefraction();
    }

    void BeforeRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void Render(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        // Shadows render pass
        RenderShadows(renderContext, node);

        // Reflection
        RenderSceneReflection(renderContext, node);

        // Refraction
        RenderSceneRefraction(renderContext, node);

        // Default Scene Render
        RenderScene(renderContext, node);

#ifndef ANDROID
        // Debug quad with shadowMap
        //RenderDebug(renderContext, node);
#endif
    }

    void PostRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(const RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        ShutDownRefraction();
        ShutDownReflection();
        ShutDownShadows();
        ShutDownDebug();
        ShutDownDefault();
    }

private:
    void InitDefault()
    {
        m_skyBoxRenderer = std::make_shared<SkyBoxRenderer>(m_defaultRenderPass);
        m_defaultRenderer = std::make_shared<DefaultRenderer>(m_defaultRenderPass);        
        m_normalMappedRenderer = std::make_shared<NormalMappedRenderer>(m_defaultRenderPass);
        m_terrainRenderer = std::make_shared<TerrainRenderer>(m_defaultRenderPass);        
        m_terrainNormalRendererRenderer = std::make_shared<TerrainNormalMappedRenderer>(m_defaultRenderPass);
        m_animationRenderer = std::make_shared<AnimationRenderer>(m_defaultRenderPass);
        m_animationNormalMappedRenderer = std::make_shared<AnimationNormalMappedRenderer>(m_defaultRenderPass);        
        m_waterRenderer = std::make_shared<WaterRenderer>(m_defaultRenderPass);
        m_fontRenderer = std::make_shared<FontRenderer>(m_defaultRenderPass);
        m_sunRenderer = std::make_shared<SunRenderer>(m_defaultRenderPass);
        m_lensFlareRenderer = std::make_shared<LensFlareRenderer>(m_defaultRenderPass);

        m_defaultRenderers = {
            m_skyBoxRenderer,
            m_defaultRenderer,
            m_normalMappedRenderer,
            m_terrainRenderer,
            m_terrainNormalRendererRenderer,
            m_animationRenderer,
            m_animationNormalMappedRenderer,
            m_waterRenderer,
            m_fontRenderer,
            m_sunRenderer,
            m_lensFlareRenderer
        };

#ifdef RENDER_BOUNDING_VOLUMES
        m_boundingVolumeDebugRenderer = std::make_shared<BoundingVolumeDebugRenderer>(m_defaultRenderPass);
        m_defaultRenderers.push_back(m_boundingVolumeDebugRenderer);
#endif

        for (auto& renderer : m_defaultRenderers) {
            renderer->Init();
        }

#ifdef PARALLEL_RENDERING
        CommandBuffersGroupFactory buffersGroupFactory{};
        m_defaultCommandBuffersGroup = buffersGroupFactory.CreateGroup(m_swapchain->GetGraphicsQueue(), m_swapchain->GetmageCount(), static_cast<uint32_t>(m_defaultRenderers.size()));
#endif
    }

    void ShutDownDefault()
    {
#ifdef PARALLEL_RENDERING
        m_defaultCommandBuffersGroup = nullptr;
#endif

        for (auto it = m_defaultRenderers.rbegin(); it != m_defaultRenderers.rend(); ++it) {
            (*it)->ShutDown();
        }
    }

    void InitDebug()
    {
        m_shadowMapDebugRenderer = std::make_shared<ShadowMapDebugRenderer>(m_defaultRenderPass);        
        m_textureDebugRenderer = std::make_shared<TextureDebugRenderer>(m_defaultRenderPass);
       
        m_debugRenderers = {
            m_shadowMapDebugRenderer,
            m_textureDebugRenderer
        };

        for (auto& renderer : m_debugRenderers) {
            renderer->Init();
        }

#ifdef PARALLEL_RENDERING
        CommandBuffersGroupFactory buffersGroupFactory{};
        m_debugCommandBuffersGroup = buffersGroupFactory.CreateGroup(m_swapchain->GetGraphicsQueue(), m_swapchain->GetmageCount(), static_cast<uint32_t>(m_debugRenderers.size()));
#endif
    }

    void ShutDownDebug()
    {
#ifdef PARALLEL_RENDERING
        m_debugCommandBuffersGroup = nullptr;
#endif

        for (auto it = m_debugRenderers.rbegin(); it != m_debugRenderers.rend(); ++it) {
            (*it)->ShutDown();
        }
    }

    void InitShadows()
    {
        const auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>(TagSet{ TAG_SHADOW });
        
        m_defaultShadowsRenderer = std::make_shared<DefaultShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_normalMappedShadowsRenderer = std::make_shared<NormalMappedShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_terrainShadowsRenderer = std::make_shared<TerrainShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_terrainNormalMappedShadowsRenderer = std::make_shared<TerrainNormalMappedShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_animationShadowsRenderer = std::make_shared<AnimationShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_animationNormalMappedShadowsRenderer = std::make_shared<AnimationNormalMappedShadowsRenderer>(shadowsComponent->GetRenderPass());
        
        m_shadowRenderers = {
            m_defaultShadowsRenderer,
            m_normalMappedShadowsRenderer,
            m_terrainShadowsRenderer,
            m_terrainNormalMappedShadowsRenderer,
            m_animationShadowsRenderer,
            m_animationNormalMappedShadowsRenderer
        };
        
        for (auto& shadowRenderer : m_shadowRenderers) {
            shadowRenderer->Init();
        }

#ifdef PARALLEL_RENDERING
        CommandBuffersGroupFactory buffersGroupFactory{};
        for (uint32_t i = 0; i < ShadowsComponent::CASCADES_COUNT; i++) {
            m_shadowsCommandBufferGroups.emplace_back(buffersGroupFactory.CreateGroup(m_swapchain->GetGraphicsQueue(), m_swapchain->GetmageCount(), static_cast<uint32_t>(m_shadowRenderers.size())));
        }
#endif
    }

    void ShutDownShadows()
    {
#ifdef PARALLEL_RENDERING
        m_shadowsCommandBufferGroups.clear();
#endif

        for (auto it = m_shadowRenderers.rbegin(); it != m_shadowRenderers.rend(); ++it) {
            (*it)->ShutDown();
        }
    }

    void InitReflection()
    {
        const auto reflectionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFLECTION_RENDER_COMPONENT });
        m_reflectionSkyBoxRenderer = std::make_shared<SkyBoxRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionDefaultRenderer = std::make_shared<DefaultRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionNormalMappedRenderer = std::make_shared<NormalMappedRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionTerrainRenderer = std::make_shared<TerrainRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionTerrainNormalMappedRenderer = std::make_shared<TerrainNormalMappedRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionAnimationRenderer = std::make_shared<AnimationRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionAnimationNormalMappedRenderer = std::make_shared<AnimationNormalMappedRenderer>(reflectionComponent->GetRenderPass());

        m_reflectionRenderers = {
            m_reflectionSkyBoxRenderer,
            m_reflectionDefaultRenderer,
            m_reflectionNormalMappedRenderer,
            m_reflectionTerrainRenderer,
            m_reflectionTerrainNormalMappedRenderer,
            m_reflectionAnimationRenderer,
            m_reflectionAnimationNormalMappedRenderer
        };

        for (auto& shadowRenderer : m_reflectionRenderers) {
            shadowRenderer->Init();
        }

#ifdef PARALLEL_RENDERING
        CommandBuffersGroupFactory buffersGroupFactory{};
        m_reflectionCommandBufferGroups = buffersGroupFactory.CreateGroup(m_swapchain->GetGraphicsQueue(), m_swapchain->GetmageCount(), static_cast<uint32_t>(m_reflectionRenderers.size()));
#endif
    }

    void ShutDownReflection()
    {
#ifdef PARALLEL_RENDERING
        m_reflectionCommandBufferGroups = nullptr;
#endif

        for (auto it = m_reflectionRenderers.rbegin(); it != m_reflectionRenderers.rend(); ++it) {
            (*it)->ShutDown();
        }
    }

    void InitRefraction()
    {
        const auto refractionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFRACTION_RENDER_COMPONENT });
        m_refractionSkyBoxRenderer = std::make_shared<SkyBoxRenderer>(refractionComponent->GetRenderPass());
        m_refractionDefaultRenderer = std::make_shared<DefaultRenderer>(refractionComponent->GetRenderPass());
        m_refractionNormalMappedRenderer = std::make_shared<NormalMappedRenderer>(refractionComponent->GetRenderPass());
        m_refractionTerrainRenderer = std::make_shared<TerrainRenderer>(refractionComponent->GetRenderPass());
        m_refractionTerrainNormalMappedRenderer = std::make_shared<TerrainNormalMappedRenderer>(refractionComponent->GetRenderPass());
        m_refractionAnimationRenderer = std::make_shared<AnimationRenderer>(refractionComponent->GetRenderPass());
        m_refractionAnimationNormalMappedRenderer = std::make_shared<AnimationNormalMappedRenderer>(refractionComponent->GetRenderPass());

        m_refractionRenderers = {
            m_refractionSkyBoxRenderer,
            m_refractionDefaultRenderer,
            m_refractionNormalMappedRenderer,
            m_refractionTerrainRenderer,
            m_refractionTerrainNormalMappedRenderer,
            m_refractionAnimationRenderer,
            m_refractionAnimationNormalMappedRenderer
        };

        for (auto& shadowRenderer : m_refractionRenderers) {
            shadowRenderer->Init();
        }

#ifdef PARALLEL_RENDERING
        CommandBuffersGroupFactory buffersGroupFactory{};
        m_refractionCommandBufferGroups = buffersGroupFactory.CreateGroup(m_swapchain->GetGraphicsQueue(), m_swapchain->GetmageCount(), static_cast<uint32_t>(m_refractionRenderers.size()));
#endif
    }

    void ShutDownRefraction()
    {
#ifdef PARALLEL_RENDERING
        m_refractionCommandBufferGroups = nullptr;
#endif

        for (auto it = m_refractionRenderers.rbegin(); it != m_refractionRenderers.rend(); ++it) {
            (*it)->ShutDown();
        }
    }

    void RenderShadows(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        for (uint32_t cascadeIndex = 0; cascadeIndex < ShadowsComponent::CASCADES_COUNT; cascadeIndex++) {

            const auto cascade = shadows->GetCascade(cascadeIndex);

            const ShadowsRenderContextUserData userData{ cascade.viewMatrix, cascade.projectionMatrix, cascadeIndex, Frustum{ cascade.projectionMatrix, cascade.viewMatrix } };

#ifdef PARALLEL_RENDERING
            shadows->GetRenderPass()->Begin(cascade.frameBuffer, renderContext.commandBuffer, { { 0, 0 }, shadows->GetExtent() }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );

            const auto& cascadeCommandBuffers = m_shadowsCommandBufferGroups.at(cascadeIndex)->GetBuffersGroup(renderContext.frameInFlightIndex);

            std::vector<std::future<void> > tasks;
            for (size_t i = 0; i < m_shadowRenderers.size(); i++) {

                auto& renderer = m_shadowRenderers.at(i);
                auto& commandBuffer = cascadeCommandBuffers.at(i);
                
                tasks.emplace_back(m_threadPool.Enqueue([&]() {
                    VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
                    inheritanceInfo.renderPass = *shadows->GetRenderPass();
                    inheritanceInfo.framebuffer = cascade.frameBuffer;

                    VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
                    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
                    commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

                    VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

                    RenderContext customRenderContext{ cascade.frameBuffer, commandBuffer, renderContext.frameInFlightIndex, shadows->GetExtent() };

                    renderer->PreRender(customRenderContext, userData);

                    for (auto child : root->GetChildren()) {
                        renderer->Render(customRenderContext, child, userData);
                    }

                    renderer->PostRender(customRenderContext, userData);

                    VKERRCHECK(vkEndCommandBuffer(commandBuffer));
                }));
            }

            for (auto&& task : tasks) {
                task.get();
            }

            vkCmdExecuteCommands(renderContext.commandBuffer, static_cast<uint32_t>(cascadeCommandBuffers.size()), cascadeCommandBuffers.data());

            shadows->GetRenderPass()->End(renderContext.commandBuffer);
#else
            shadows->GetRenderPass()->Begin(cascade.frameBuffer, renderContext.commandBuffer, { { 0, 0 }, shadows->GetExtent() } );

            for (size_t i = 0; i < m_shadowRenderers.size(); i++) {
                auto& renderer = m_shadowRenderers.at(i);

                renderer->PreRender(renderContext, userData);

                for (auto child : root->GetChildren()) {
                    renderer->Render(renderContext, child, userData);
                }

                renderer->PostRender(renderContext, userData);
            }

            shadows->GetRenderPass()->End(renderContext.commandBuffer);
#endif
        }
    }

    void RenderSceneReflection(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
        const auto reflectionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFLECTION_RENDER_COMPONENT });
        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        const auto cameraPosition{ cameraComponent->GetPosition() };
        const auto cameraViewPosition{ cameraComponent->GetPosition() + cameraComponent->GetForwardDirection() };
        const float cameraPositionOffset{ 2.0f * (cameraPosition.y - WATER_LEVEL) };
        const float cameraViewOffset{ 2.0f * (WATER_LEVEL - cameraViewPosition.y) };

        const glm::vec3 newCameraPosition{ cameraPosition.x, cameraPosition.y - cameraPositionOffset, cameraPosition.z };
        const glm::vec3 newCameraViewPosition{ cameraViewPosition.x, cameraViewPosition.y + cameraViewOffset, cameraViewPosition.z };
        const glm::mat4 viewMatrix = glm::lookAt(newCameraPosition, newCameraViewPosition, cameraComponent->GetUpDirection());
        const glm::mat4 projectionMatrix = cameraComponent->GetViewFrustum().CreateProjectionMatrix(reflectionComponent->GetExtent().width, reflectionComponent->GetExtent().height);

        NormalRenderContextUserData userData{
            viewMatrix,
            projectionMatrix,
            newCameraPosition,
            glm::vec4(0.0f, 1.0f, 0.0f, -WATER_LEVEL + WATER_CLIP_PLANE_OFFSET),
            reflectionComponent->GetExtent(),
            glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane()),
            Frustum{ projectionMatrix, viewMatrix }
        };

#ifdef PARALLEL_RENDERING
        reflectionComponent->GetRenderPass()->Begin(reflectionComponent->GetFrameBuffer(), renderContext.commandBuffer, { { 0, 0 }, reflectionComponent->GetExtent() }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

        const auto& commandBuffers = m_reflectionCommandBufferGroups->GetBuffersGroup(renderContext.frameInFlightIndex);

        std::vector<std::future<void> > tasks;
        for (size_t i = 0; i < m_reflectionRenderers.size(); i++) {

            auto& renderer = m_reflectionRenderers.at(i);
            auto& commandBuffer = commandBuffers.at(i);

            tasks.emplace_back(m_threadPool.Enqueue([&]() {
                VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
                inheritanceInfo.renderPass = *reflectionComponent->GetRenderPass();
                inheritanceInfo.framebuffer = reflectionComponent->GetFrameBuffer();

                VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
                commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
                commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

                VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

                RenderContext customRenderContext{ renderContext.frameBuffer, commandBuffer, renderContext.frameInFlightIndex, renderContext.fullExtent };

                renderer->PreRender(customRenderContext, userData);

                for (auto child : root->GetChildren()) {
                    renderer->Render(customRenderContext, child, userData);
                }

                renderer->PostRender(customRenderContext, userData);

                VKERRCHECK(vkEndCommandBuffer(commandBuffer));
            }));
        }

        for (auto&& task : tasks) {
            task.get();
        }

        vkCmdExecuteCommands(renderContext.commandBuffer, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        reflectionComponent->GetRenderPass()->End(renderContext.commandBuffer);
#else
        reflectionComponent->GetRenderPass()->Begin(reflectionComponent->GetFrameBuffer(), renderContext.commandBuffer, { { 0, 0 }, reflectionComponent->GetExtent() });

        for (size_t i = 0; i < m_reflectionRenderers.size(); i++) {
            auto& renderer = m_reflectionRenderers.at(i);

            renderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                renderer->Render(renderContext, child, userData);
            }

            renderer->PostRender(renderContext, userData);
        }

        reflectionComponent->GetRenderPass()->End(renderContext.commandBuffer);
#endif
    }

    void RenderSceneRefraction(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
        const auto refractionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFRACTION_RENDER_COMPONENT });
        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        const auto viewMatrix = cameraComponent->LookAt();
        const auto projectionMatrix = cameraComponent->GetViewFrustum().CreateProjectionMatrix(refractionComponent->GetExtent().width, refractionComponent->GetExtent().height);
        NormalRenderContextUserData userData{
            viewMatrix,
            projectionMatrix,
            cameraComponent->GetPosition(),
            glm::vec4(0.0f, -1.0f, 0.0f, WATER_LEVEL + WATER_CLIP_PLANE_OFFSET),
            refractionComponent->GetExtent(),
            glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane()),
            Frustum{ projectionMatrix, viewMatrix }
        };

#ifdef PARALLEL_RENDERING
        const auto& commandBuffers = m_refractionCommandBufferGroups->GetBuffersGroup(renderContext.frameInFlightIndex);

        refractionComponent->GetRenderPass()->Begin(refractionComponent->GetFrameBuffer(), renderContext.commandBuffer, { { 0, 0 }, refractionComponent->GetExtent() }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

        std::vector<std::future<void> > tasks;
        for (size_t i = 0; i < m_refractionRenderers.size(); i++) {

            auto& renderer = m_refractionRenderers.at(i);
            auto& commandBuffer = commandBuffers.at(i);

            tasks.emplace_back(m_threadPool.Enqueue([&]() {
                VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
                inheritanceInfo.renderPass = *refractionComponent->GetRenderPass();
                inheritanceInfo.framebuffer = refractionComponent->GetFrameBuffer();

                VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
                commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
                commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

                VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

                RenderContext customRenderContext{ renderContext.frameBuffer, commandBuffer, renderContext.frameInFlightIndex, renderContext.fullExtent };

                renderer->PreRender(customRenderContext, userData);

                for (auto child : root->GetChildren()) {
                    renderer->Render(customRenderContext, child, userData);
                }

                renderer->PostRender(customRenderContext, userData);

                VKERRCHECK(vkEndCommandBuffer(commandBuffer));
            }));
        }

        for (auto&& task : tasks) {
            task.get();
        }

        vkCmdExecuteCommands(renderContext.commandBuffer, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        refractionComponent->GetRenderPass()->End(renderContext.commandBuffer);
#else
        refractionComponent->GetRenderPass()->Begin(refractionComponent->GetFrameBuffer(), renderContext.commandBuffer, { { 0, 0 }, refractionComponent->GetExtent() });

        for (size_t i = 0; i < m_refractionRenderers.size(); i++) {
            auto& renderer = m_refractionRenderers.at(i);

            renderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                renderer->Render(renderContext, child, userData);
            }

            renderer->PostRender(renderContext, userData);
        }

        refractionComponent->GetRenderPass()->End(renderContext.commandBuffer);
#endif
    }

    void RenderScene(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        const auto viewMatrix = cameraComponent->LookAt();
        const auto projectionMatrix = cameraComponent->GetViewFrustum().CreateProjectionMatrix(renderContext.fullExtent.width, renderContext.fullExtent.height);
        NormalRenderContextUserData userData{
            viewMatrix,
            projectionMatrix,
            cameraComponent->GetPosition(),
            DEFAULT_CLIP_PLANE,
            renderContext.fullExtent,
            glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane()),
            Frustum{ projectionMatrix, viewMatrix }
        };

        m_sunRenderer->BeforeRender(renderContext, userData);

#ifdef PARALLEL_RENDERING
        m_defaultRenderPass->Begin(renderContext.frameBuffer, renderContext.commandBuffer, { { 0, 0 }, renderContext.fullExtent }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

        const auto& commandBuffers = m_defaultCommandBuffersGroup->GetBuffersGroup(renderContext.frameInFlightIndex);

        std::vector<std::future<void> > tasks;
        for (size_t i = 0; i < m_defaultRenderers.size(); i++) {
            
            auto& renderer = m_defaultRenderers.at(i);
            auto& commandBuffer = commandBuffers.at(i);

            tasks.emplace_back(m_threadPool.Enqueue([&]() {
                VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
                inheritanceInfo.renderPass = *m_defaultRenderPass;
                inheritanceInfo.framebuffer = renderContext.frameBuffer;

                VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
                commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
                commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

                VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

                RenderContext customRenderContext{ renderContext.frameBuffer, commandBuffer, renderContext.frameInFlightIndex, renderContext.fullExtent };

                renderer->PreRender(customRenderContext, userData);

                for (auto child : root->GetChildren()) {
                    renderer->Render(customRenderContext, child, userData);
                }

                renderer->PostRender(customRenderContext, userData);

                VKERRCHECK(vkEndCommandBuffer(commandBuffer));
            }));
        }

        for (auto&& task : tasks) {
            task.get();
        }

        vkCmdExecuteCommands(renderContext.commandBuffer, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        m_defaultRenderPass->End(renderContext.commandBuffer);
#else
        m_defaultRenderPass->Begin(renderContext.frameBuffer, renderContext.commandBuffer, { { 0, 0 }, renderContext.fullExtent });

        for (size_t i = 0; i < m_defaultRenderers.size(); i++) {
            auto& renderer = m_defaultRenderers.at(i);

            renderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                renderer->Render(renderContext, child, userData);
            }

            renderer->PostRender(renderContext, userData);
        }

        m_defaultRenderPass->End(renderContext.commandBuffer);
#endif
        m_sunRenderer->AfterRender(renderContext, userData);       
    }

    void RenderDebug(const RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
#ifdef PARALLEL_RENDERING
        m_defaultRenderPass->Begin(renderContext.frameBuffer, renderContext.commandBuffer, { { 0, 0 }, { renderContext.fullExtent.width / 2, renderContext.fullExtent.height / 2 } }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

        const auto& debugCommandBuffers = m_debugCommandBuffersGroup->GetBuffersGroup(renderContext.frameInFlightIndex);

        std::vector<std::future<void> > debugTasks;
        for (size_t i = 0; i < m_debugRenderers.size(); i++) {

            auto& renderer = m_debugRenderers.at(i);
            auto& commandBuffer = debugCommandBuffers.at(i);

            debugTasks.emplace_back(m_threadPool.Enqueue([&]() {
                VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
                inheritanceInfo.renderPass = *m_defaultRenderPass;
                inheritanceInfo.framebuffer = renderContext.frameBuffer;

                VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
                commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
                commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

                VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

                RenderContext customRenderContext{ renderContext.frameBuffer, commandBuffer, renderContext.frameInFlightIndex, renderContext.fullExtent };

                renderer->PreRender(customRenderContext);

                renderer->Render(customRenderContext, root);

                renderer->PostRender(customRenderContext);

                VKERRCHECK(vkEndCommandBuffer(commandBuffer));
            }));
        }

        for (auto&& debugTask : debugTasks) {
            debugTask.get();
        }

        vkCmdExecuteCommands(renderContext.commandBuffer, static_cast<uint32_t>(debugCommandBuffers.size()), debugCommandBuffers.data());

        m_defaultRenderPass->End(renderContext.commandBuffer);
#else
        m_defaultRenderPass->Begin(renderContext.frameBuffer, renderContext.commandBuffer, { { 0, 0 }, { renderContext.fullExtent.width / 2, renderContext.fullExtent.height / 2 } });

        for (size_t i = 0; i < m_debugRenderers.size(); i++) {
            auto& renderer = m_debugRenderers.at(i);

            renderer->PreRender(renderContext);

            renderer->Render(renderContext, root);

            renderer->PostRender(renderContext);
        }

        m_defaultRenderPass->End(renderContext.commandBuffer);
#endif
    }

private:
    std::shared_ptr<RenderPass> m_defaultRenderPass;

    std::shared_ptr<Swapchain> m_swapchain;

private:
    // Default Render
    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_skyBoxRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_defaultRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_normalMappedRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_terrainRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_terrainNormalRendererRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_animationRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_animationNormalMappedRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_waterRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_fontRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_sunRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_lensFlareRenderer;
#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_boundingVolumeDebugRenderer;
#endif
    std::vector<std::shared_ptr<IRenderer<NormalRenderContextUserData> > > m_defaultRenderers;


    // Debug
    std::shared_ptr<IRenderer<DefaultRenderContextUserData> > m_shadowMapDebugRenderer;

    std::shared_ptr<IRenderer<DefaultRenderContextUserData> > m_textureDebugRenderer;

    std::vector<std::shared_ptr<IRenderer<DefaultRenderContextUserData> > > m_debugRenderers;


    // Shadows
    std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > m_defaultShadowsRenderer;

    std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > m_normalMappedShadowsRenderer;

    std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > m_terrainShadowsRenderer;

    std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > m_terrainNormalMappedShadowsRenderer;

    std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > m_animationShadowsRenderer;

    std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > m_animationNormalMappedShadowsRenderer;

    std::vector<std::shared_ptr<IRenderer<ShadowsRenderContextUserData> > > m_shadowRenderers;


    // Reflection
    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionSkyBoxRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionDefaultRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionNormalMappedRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionTerrainRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionTerrainNormalMappedRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionAnimationRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionAnimationNormalMappedRenderer;
    
    std::vector<std::shared_ptr<IRenderer<NormalRenderContextUserData> > > m_reflectionRenderers;


    // Refraction
    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_refractionSkyBoxRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_refractionDefaultRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_refractionNormalMappedRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_refractionTerrainRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_refractionTerrainNormalMappedRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_refractionAnimationRenderer;

    std::shared_ptr<IRenderer<NormalRenderContextUserData> > m_refractionAnimationNormalMappedRenderer;

    std::vector<std::shared_ptr<IRenderer<NormalRenderContextUserData> > > m_refractionRenderers;

#ifdef PARALLEL_RENDERING
    // Parallel stuff
    std::unique_ptr<CommandBuffersGroup> m_defaultCommandBuffersGroup;

    std::unique_ptr<CommandBuffersGroup> m_debugCommandBuffersGroup;

    std::vector<std::unique_ptr<CommandBuffersGroup> > m_shadowsCommandBufferGroups;

    std::unique_ptr<CommandBuffersGroup> m_reflectionCommandBufferGroups;

    std::unique_ptr<CommandBuffersGroup> m_refractionCommandBufferGroups;

    ThreadPool m_threadPool{ 8 };
#endif
};

#endif