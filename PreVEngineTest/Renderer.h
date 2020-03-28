#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "General.h"

struct ShadowsRenderContextUserData : DefaultRenderContextUserData {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const uint32_t cascadeIndex;

    ShadowsRenderContextUserData(const glm::mat4& vm, const glm::mat4& pm, const uint32_t index)
        : viewMatrix(vm)
        , projectionMatrix(pm)
        , cascadeIndex(index)
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

    NormalRenderContextUserData(const glm::mat4& vm, const glm::mat4& pm, const glm::vec3& camPos, const glm::vec4& cp, const VkExtent2D& ext, const glm::vec2& nearFar)
        : viewMatrix(vm)
        , projectionMatrix(pm)
        , cameraPosition(camPos)
        , clipPlane(cp)
        , extent(ext)
        , nearFarClippingPlane(nearFar)
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
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<Uniforms> > m_uniformsPool;

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

        m_pipeline = std::make_shared<DefaultShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Default Shadows Pipeline created\n");

        m_uniformsPool = std::make_shared<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void PreRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void BeforeRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT })) {
            auto renderComponent = ComponentRepository<IRenderComponent>::Instance().Get(node->GetId());
            if (renderComponent->CastsShadows()) {
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

                vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
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
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<Uniforms> > m_uniformsPool;

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

        m_pipeline = std::make_shared<NormalMappedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("NormalMapped Shadows Pipeline created\n");

        m_uniformsPool = std::make_shared<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void PreRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void BeforeRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_NORMAL_MAPPED_COMPONENT })) {
            auto renderComponent = ComponentRepository<IRenderComponent>::Instance().Get(node->GetId());
            if (renderComponent->CastsShadows()) {
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

                vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
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
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<Uniforms> > m_uniformsPool;

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

        m_pipeline = std::make_shared<TerrainShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Terrain Shadows Pipeline created\n");

        m_uniformsPool = std::make_shared<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void PreRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_RENDER_COMPONENT })) {
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
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
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<Uniforms> > m_uniformsPool;

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

        m_pipeline = std::make_shared<TerrainNormalMappedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Terrain Normal Mapped Shadows Pipeline created\n");

        m_uniformsPool = std::make_shared<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void PreRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT })) {
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
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
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<Uniforms> > m_uniformsPool;

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

        m_pipeline = std::make_shared<AnimatedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Animation Shadows Pipeline created\n");

        m_uniformsPool = std::make_shared<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void PreRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_RENDER_COMPONENT })) {
            auto renderComponent = ComponentRepository<IAnimationRenderComponent>::Instance().Get(node->GetId());
            if (renderComponent->CastsShadows()) {
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

                vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
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
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<IGraphicsPipeline> m_pipeline;

    std::shared_ptr<UBOPool<Uniforms> > m_uniformsPool;

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

        m_pipeline = std::make_shared<AnimatedNormalMappedShadowsPipeline>(*device, *m_renderPass, *m_shader);
        m_pipeline->Init();

        LOGI("Animation Normal Mapped Shadows Pipeline created\n");

        m_uniformsPool = std::make_shared<UBOPool<Uniforms> >(*allocator);
        m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
    }

    void BeforeRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void PreRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto shadowsExtent = shadows->GetExtent();

        VkRect2D scissor = { { 0, 0 }, shadows->GetExtent() };
        VkViewport viewport = { 0, 0, static_cast<float>(shadowsExtent.width), static_cast<float>(shadowsExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT })) {
            auto renderComponent = ComponentRepository<IAnimationRenderComponent>::Instance().Get(node->GetId());
            if (renderComponent->CastsShadows()) {
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

                vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, shadowsRenderContext);
        }
    }

    void PostRender(RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override
    {
    }

    void AfterRender(RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override
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
        auto quadMesh = std::make_shared<QuadMesh>();

        auto vertexBuffer = std::make_shared<VBO>(*allocator);
        vertexBuffer->Data(quadMesh->GetVertices(), quadMesh->GerVerticesCount(), quadMesh->GetVertexLayout().GetStride());

        auto indexBuffer = std::make_shared<IBO>(*allocator);
        indexBuffer->Data(quadMesh->GerIndices().data(), (uint32_t)quadMesh->GerIndices().size());

        m_quadModel = std::make_unique<Model>(quadMesh, vertexBuffer, indexBuffer);
    }

    void BeforeRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D renderRect{};
        renderRect.extent.width = renderContext.fullExtent.width / 2;
        renderRect.extent.height = renderContext.fullExtent.height / 2;
        renderRect.offset.x = 0;
        renderRect.offset.y = 0;

        m_renderPass->Begin(renderContext.defaultFrameBuffer, renderContext.defaultCommandBuffer, renderRect);

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

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    // make a node with quad model & shadowMap texture ???
    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        const auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });

        const auto& cascade = shadows->GetCascade(m_cascadeIndex);
        PushConstantBlock pushConstBlock{ static_cast<uint32_t>(m_cascadeIndex), -cascade.startSplitDepth, -cascade.endSplitDepth };
        vkCmdPushConstants(renderContext.defaultCommandBuffer, m_pipeline->GetLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantBlock), &pushConstBlock);

        m_shader->Bind("depthSampler", shadows->GetImageBuffer()->GetImageView(), shadows->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

        VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();

        VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.defaultCommandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        m_renderPass->End(renderContext.defaultCommandBuffer);
    }

    void AfterRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
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
        auto quadMesh = std::make_shared<QuadMesh>();

        auto vertexBuffer = std::make_shared<VBO>(*allocator);
        vertexBuffer->Data(quadMesh->GetVertices(), quadMesh->GerVerticesCount(), quadMesh->GetVertexLayout().GetStride());

        auto indexBuffer = std::make_shared<IBO>(*allocator);
        indexBuffer->Data(quadMesh->GerIndices().data(), (uint32_t)quadMesh->GerIndices().size());

        m_quadModel = std::make_unique<Model>(quadMesh, vertexBuffer, indexBuffer);
    }

    void BeforeRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D renderRect{};
        renderRect.extent.width = renderContext.fullExtent.width / 2;
        renderRect.extent.height = renderContext.fullExtent.height / 2;
        renderRect.offset.x = 0;
        renderRect.offset.y = 0;

        m_renderPass->Begin(renderContext.defaultFrameBuffer, renderContext.defaultCommandBuffer, renderRect);

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

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    // make a node with quad model & shadowMap texture ???
    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        const auto component = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFLECTION_RENDER_COMPONENT });

        m_shader->Bind("imageSampler", component->GetColorImageBuffer()->GetImageView(), component->GetColorImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();

        VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.defaultCommandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        m_renderPass->End(renderContext.defaultCommandBuffer);
    }

    void AfterRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

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

    void BeforeRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT })) {
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
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

    void BeforeRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_NORMAL_MAPPED_COMPONENT })) {
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
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

    void BeforeRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_RENDER_COMPONENT })) {
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
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

    void BeforeRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT })) {
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

            m_shader->Bind("textureSampler", *nodeRenderComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("normalSampler", *nodeRenderComponent->GetMaterial()->GetNormalmageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
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

    void BeforeRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_RENDER_COMPONENT })) {
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
            uniformsFS.minHeight = terrainComponent->GetHeightMapInfo()->GetMinHeight();
            uniformsFS.maxHeight = terrainComponent->GetHeightMapInfo()->GetMaxHeight();
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
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

    void BeforeRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT })) {
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
            uniformsFS.minHeight = terrainComponent->GetHeightMapInfo()->GetMinHeight();
            uniformsFS.maxHeight = terrainComponent->GetHeightMapInfo()->GetMaxHeight();
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        m_shader->ShutDown();

        m_pipeline->ShutDown();
    }
};

class FontRenderer : public IRenderer<DefaultRenderContextUserData> {
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

    void BeforeRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
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

                vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderableText.model->GetIndexBuffer(), 0, renderableText.model->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderableText.model->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
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

    void BeforeRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *skyBoxComponent->GetModel()->GetIndexBuffer(), 0, skyBoxComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, skyBoxComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
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

    void PreRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void BeforeRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_RENDER_COMPONENT })) {
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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *waterComponent->GetModel()->GetIndexBuffer(), 0, waterComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, waterComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override
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

class SunRenderer : public IRenderer<DefaultRenderContextUserData> {
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

    void BeforeRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        m_passedSamples = 0;
        vkCmdResetQueryPool(renderContext.defaultCommandBuffer, m_queryPool, 0, 1);
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
    {
        if (node->GetFlags().HasAll(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_SUN_RENDER_COMPONENT })) {
            const auto sunComponent = ComponentRepository<ISunComponent>::Instance().Get(node->GetId());

            const float xScale = sunComponent->GetFlare()->GetScale();
            const float yScale = xScale * renderContext.fullExtent.width / renderContext.fullExtent.height;

            m_maxNumberOfSamples = static_cast<uint64_t>(powf(sunComponent->GetFlare()->GetScale() * renderContext.fullExtent.width, 2.0f));

            vkCmdBeginQuery(renderContext.defaultCommandBuffer, m_queryPool, 0, 0);

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

            vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *sunComponent->GetModel()->GetIndexBuffer(), 0, sunComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.defaultCommandBuffer, sunComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);

            vkCmdEndQuery(renderContext.defaultCommandBuffer, m_queryPool, 0);
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        auto device = DeviceProvider::Instance().GetDevice();

#if defined(__ANDROID__)
        auto result = vkGetQueryPoolResults(*device, m_queryPool, 0, 1, sizeof(m_passedSamples), &m_passedSamples, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_PARTIAL_BIT);
#else
        auto result = vkGetQueryPoolResults(*device, m_queryPool, 0, 1, sizeof(m_passedSamples), &m_passedSamples, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
#endif
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

class LensFlareRenderer : public IRenderer<DefaultRenderContextUserData> {
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

    void BeforeRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
        VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
        VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

        vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
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

                vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *lensFlareComponent->GetModel()->GetIndexBuffer(), 0, lensFlareComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.defaultCommandBuffer, lensFlareComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            }
        }

        for (auto child : node->GetChildren()) {
            Render(renderContext, child, renderContextUserData);
        }
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
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

class MasterRenderer : public IRenderer<DefaultRenderContextUserData> {
public:
    MasterRenderer(const std::shared_ptr<RenderPass>& renderPass)
        : m_defaultRenderPass(renderPass)
    {
    }

    ~MasterRenderer() = default;

public:
    void Init() override
    {
        InitDefault();
        InitShadows();
        InitReflection();
        InitRefraction();
    }

    void BeforeRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void PreRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void Render(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override
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
        //m_shadowMapDebugRenderer->PreRender(renderContext);

        //m_shadowMapDebugRenderer->Render(renderContext, node);

        //m_shadowMapDebugRenderer->PostRender(renderContext);

        //m_textureDebugRenderer->PreRender(renderContext);

        //m_textureDebugRenderer->Render(renderContext, node);

        //m_textureDebugRenderer->PostRender(renderContext);
#endif
    }

    void PostRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void AfterRender(RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override
    {
    }

    void ShutDown() override
    {
        ShutDownRefraction();
        ShutDownReflection();
        ShutDownShadows();
        ShutDownDefault();
    }

private:
    void InitDefault()
    {
        m_skyboxRenderer = std::make_unique<SkyBoxRenderer>(m_defaultRenderPass);
        m_skyboxRenderer->Init();

        m_defaultRenderer = std::make_unique<DefaultRenderer>(m_defaultRenderPass);
        m_defaultRenderer->Init();

        m_normalMappedRenderer = std::make_unique<NormalMappedRenderer>(m_defaultRenderPass);
        m_normalMappedRenderer->Init();

        m_terrainRenderer = std::make_unique<TerrainRenderer>(m_defaultRenderPass);
        m_terrainRenderer->Init();

        m_terrainNormalRendererRenderer = std::make_unique<TerrainNormalMappedRenderer>(m_defaultRenderPass);
        m_terrainNormalRendererRenderer->Init();

        m_animationRenderer = std::make_unique<AnimationRenderer>(m_defaultRenderPass);
        m_animationRenderer->Init();

        m_animationNormalMappedRenderer = std::make_unique<AnimationNormalMappedRenderer>(m_defaultRenderPass);
        m_animationNormalMappedRenderer->Init();

        m_waterRenderer = std::make_unique<WaterRenderer>(m_defaultRenderPass);
        m_waterRenderer->Init();

        m_fontRenderer = std::make_unique<FontRenderer>(m_defaultRenderPass);
        m_fontRenderer->Init();

        m_sunRenderer = std::make_unique<SunRenderer>(m_defaultRenderPass);
        m_sunRenderer->Init();

        m_lensFlareRenderer = std::make_unique<LensFlareRenderer>(m_defaultRenderPass);
        m_lensFlareRenderer->Init();

        m_shadowMapDebugRenderer = std::make_unique<ShadowMapDebugRenderer>(m_defaultRenderPass);
        m_shadowMapDebugRenderer->Init();

        m_textureDebugRenderer = std::make_unique<TextureDebugRenderer>(m_defaultRenderPass);
        m_textureDebugRenderer->Init();
    }

    void InitShadows()
    {
        auto shadowsComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>(TagSet{ TAG_SHADOW });
        m_defaultShadowsRenderer = std::make_unique<DefaultShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_defaultShadowsRenderer->Init();

        m_normalMappedShadowsRenderer = std::make_unique<NormalMappedShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_normalMappedShadowsRenderer->Init();

        m_terrainShadowsRenderer = std::make_unique<TerrainShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_terrainShadowsRenderer->Init();

        m_terrainNormalMappedShadowsRenderer = std::make_unique<TerrainNormalMappedShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_terrainNormalMappedShadowsRenderer->Init();

        m_animationShadowsRenderer = std::make_unique<AnimationShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_animationShadowsRenderer->Init();

        m_animationNormalMappedShadowsRenderer = std::make_unique<AnimationNormalMappedShadowsRenderer>(shadowsComponent->GetRenderPass());
        m_animationNormalMappedShadowsRenderer->Init();
    }

    void InitReflection()
    {
        auto reflectionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFLECTION_RENDER_COMPONENT });
        m_reflectionSkyBoxRenderer = std::make_unique<SkyBoxRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionSkyBoxRenderer->Init();

        m_reflectionDefaultRenderer = std::make_unique<DefaultRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionDefaultRenderer->Init();

        m_reflectionNormalMappedRenderer = std::make_unique<NormalMappedRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionNormalMappedRenderer->Init();

        m_reflectionTerrainRenderer = std::make_unique<TerrainRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionTerrainRenderer->Init();

        m_reflectionTerrainNormalMappedRenderer = std::make_unique<TerrainNormalMappedRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionTerrainNormalMappedRenderer->Init();

        m_reflectionAnimationRenderer = std::make_unique<AnimationRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionAnimationRenderer->Init();

        m_reflectionAnimationNormalMappedRenderer = std::make_unique<AnimationNormalMappedRenderer>(reflectionComponent->GetRenderPass());
        m_reflectionAnimationNormalMappedRenderer->Init();
    }

    void InitRefraction()
    {
        auto refractionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFRACTION_RENDER_COMPONENT });
        m_refractionSkyBoxRenderer = std::make_unique<SkyBoxRenderer>(refractionComponent->GetRenderPass());
        m_refractionSkyBoxRenderer->Init();

        m_refractionDefaultRenderer = std::make_unique<DefaultRenderer>(refractionComponent->GetRenderPass());
        m_refractionDefaultRenderer->Init();

        m_refractionNormalMappedRenderer = std::make_unique<NormalMappedRenderer>(refractionComponent->GetRenderPass());
        m_refractionNormalMappedRenderer->Init();

        m_refractionTerrainRenderer = std::make_unique<TerrainRenderer>(refractionComponent->GetRenderPass());
        m_refractionTerrainRenderer->Init();

        m_refractionTerrainNormalMappedRenderer = std::make_unique<TerrainNormalMappedRenderer>(refractionComponent->GetRenderPass());
        m_refractionTerrainNormalMappedRenderer->Init();

        m_refractionAnimationRenderer = std::make_unique<AnimationRenderer>(refractionComponent->GetRenderPass());
        m_refractionAnimationRenderer->Init();

        m_refractionAnimationNormalMappedRenderer = std::make_unique<AnimationNormalMappedRenderer>(refractionComponent->GetRenderPass());
        m_refractionAnimationNormalMappedRenderer->Init();
    }

    void ShutDownRefraction()
    {
        m_refractionAnimationNormalMappedRenderer->ShutDown();
        m_refractionAnimationRenderer->ShutDown();
        m_refractionTerrainNormalMappedRenderer->ShutDown();
        m_refractionTerrainRenderer->ShutDown();
        m_refractionNormalMappedRenderer->ShutDown();
        m_refractionDefaultRenderer->ShutDown();
        m_reflectionSkyBoxRenderer->ShutDown();
    }

    void ShutDownReflection()
    {
        m_reflectionAnimationNormalMappedRenderer->ShutDown();
        m_reflectionAnimationRenderer->ShutDown();
        m_reflectionTerrainNormalMappedRenderer->ShutDown();
        m_reflectionTerrainRenderer->ShutDown();
        m_reflectionNormalMappedRenderer->ShutDown();
        m_reflectionDefaultRenderer->ShutDown();
        m_reflectionSkyBoxRenderer->ShutDown();
    }

    void ShutDownShadows()
    {
        m_animationNormalMappedShadowsRenderer->ShutDown();
        m_animationShadowsRenderer->ShutDown();
        m_terrainNormalMappedShadowsRenderer->ShutDown();
        m_terrainShadowsRenderer->ShutDown();
        m_normalMappedShadowsRenderer->ShutDown();
        m_defaultShadowsRenderer->ShutDown();
    }

    void ShutDownDefault()
    {
        m_textureDebugRenderer->ShutDown();
        m_shadowMapDebugRenderer->ShutDown();
        m_lensFlareRenderer->ShutDown();
        m_sunRenderer->ShutDown();
        m_fontRenderer->ShutDown();
        m_waterRenderer->ShutDown();
        m_animationNormalMappedRenderer->ShutDown();
        m_animationRenderer->ShutDown();
        m_terrainNormalRendererRenderer->ShutDown();
        m_terrainRenderer->ShutDown();
        m_normalMappedRenderer->ShutDown();
        m_defaultRenderer->ShutDown();
        m_skyboxRenderer->ShutDown();
    }

    void RenderShadows(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
        auto shadows = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IShadowsComponent>({ TAG_SHADOW });
        for (uint32_t cascadeIndex = 0; cascadeIndex < ShadowsComponent::CASCADES_COUNT; cascadeIndex++) {

            auto cascade = shadows->GetCascade(cascadeIndex);
            shadows->GetRenderPass()->Begin(cascade.frameBuffer, renderContext.defaultCommandBuffer, { { 0, 0 }, shadows->GetExtent() });

            ShadowsRenderContextUserData userData{
                cascade.viewMatrix,
                cascade.projectionMatrix,
                cascadeIndex,
            };

            // Default
            m_defaultShadowsRenderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                m_defaultShadowsRenderer->Render(renderContext, child, userData);
            }

            m_defaultShadowsRenderer->PostRender(renderContext, userData);

            // Normal mapped
            m_normalMappedShadowsRenderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                m_normalMappedShadowsRenderer->Render(renderContext, child, userData);
            }

            m_normalMappedShadowsRenderer->PostRender(renderContext, userData);

            // Terrain
            m_terrainShadowsRenderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                m_terrainShadowsRenderer->Render(renderContext, child, userData);
            }

            m_terrainShadowsRenderer->PostRender(renderContext, userData);
            
            // Terrain Normal Mapped
            m_terrainNormalMappedShadowsRenderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                m_terrainNormalMappedShadowsRenderer->Render(renderContext, child, userData);
            }

            m_terrainNormalMappedShadowsRenderer->PostRender(renderContext, userData);

            // Animation
            m_animationShadowsRenderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                m_animationShadowsRenderer->Render(renderContext, child, userData);
            }

            m_animationShadowsRenderer->PostRender(renderContext, userData);

            // Animation Normal Mapped
            m_animationNormalMappedShadowsRenderer->PreRender(renderContext, userData);

            for (auto child : root->GetChildren()) {
                m_animationNormalMappedShadowsRenderer->Render(renderContext, child, userData);
            }

            m_animationNormalMappedShadowsRenderer->PostRender(renderContext, userData);

            shadows->GetRenderPass()->End(renderContext.defaultCommandBuffer);
        }
    }

    void RenderSceneReflection(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
        auto reflectionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFLECTION_RENDER_COMPONENT });
        reflectionComponent->GetRenderPass()->Begin(reflectionComponent->GetFrameBuffer(), renderContext.defaultCommandBuffer, { { 0, 0 }, { REFLECTION_MEASURES.x, REFLECTION_MEASURES.y } });

        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        const auto cameraPosition{ cameraComponent->GetPosition() };
        const auto cameraViewPosition{ cameraComponent->GetPosition() + cameraComponent->GetForwardDirection() };
        const float cameraPositionOffset{ 2.0f * (cameraPosition.y - WATER_LEVEL) };
        const float cameraViewOffset{ 2.0f * (WATER_LEVEL - cameraViewPosition.y) };

        const glm::vec3 newCameraPosition{ cameraPosition.x, cameraPosition.y - cameraPositionOffset, cameraPosition.z };
        const glm::vec3 newCameraViewPosition{ cameraViewPosition.x, cameraViewPosition.y + cameraViewOffset, cameraViewPosition.z };
        const glm::mat4 viewMatrix = glm::lookAt(newCameraPosition, newCameraViewPosition, cameraComponent->GetUpDirection());

        NormalRenderContextUserData userData{
            viewMatrix,
            cameraComponent->GetViewFrustum().CreateProjectionMatrix(REFLECTION_MEASURES.x, REFLECTION_MEASURES.y),
            newCameraPosition,
            glm::vec4(0.0f, 1.0f, 0.0f, -WATER_LEVEL + WATER_CLIP_PLANE_OFFSET),
            { REFLECTION_MEASURES.x, REFLECTION_MEASURES.y },
            glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane())
        };

        // SkyBox
        m_reflectionSkyBoxRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_reflectionSkyBoxRenderer->Render(renderContext, child, userData);
        }

        m_reflectionSkyBoxRenderer->PostRender(renderContext, userData);

        // Default
        m_reflectionDefaultRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_reflectionDefaultRenderer->Render(renderContext, child, userData);
        }

        m_reflectionDefaultRenderer->PostRender(renderContext, userData);

        // NormalMapped
        m_reflectionNormalMappedRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_reflectionNormalMappedRenderer->Render(renderContext, child, userData);
        }

        m_reflectionNormalMappedRenderer->PostRender(renderContext, userData);

        // Terrain
        m_reflectionTerrainRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_reflectionTerrainRenderer->Render(renderContext, child, userData);
        }

        m_reflectionTerrainRenderer->PostRender(renderContext, userData);
        
        // Terrain Normal Mapped
        m_reflectionTerrainNormalMappedRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_reflectionTerrainNormalMappedRenderer->Render(renderContext, child, userData);
        }

        m_reflectionTerrainNormalMappedRenderer->PostRender(renderContext, userData);

        // Animation
        m_reflectionAnimationRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_reflectionAnimationRenderer->Render(renderContext, child, userData);
        }

        m_reflectionAnimationRenderer->PostRender(renderContext, userData);

        // Animation Normal Mapped
        m_reflectionAnimationNormalMappedRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_reflectionAnimationNormalMappedRenderer->Render(renderContext, child, userData);
        }

        m_reflectionAnimationNormalMappedRenderer->PostRender(renderContext, userData);

        reflectionComponent->GetRenderPass()->End(renderContext.defaultCommandBuffer);
    }

    void RenderSceneRefraction(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
        auto refractionComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFRACTION_RENDER_COMPONENT });
        refractionComponent->GetRenderPass()->Begin(refractionComponent->GetFrameBuffer(), renderContext.defaultCommandBuffer, { { 0, 0 }, { REFRACTION_MEASURES.x, REFRACTION_MEASURES.y } });

        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        NormalRenderContextUserData userData{
            cameraComponent->LookAt(),
            cameraComponent->GetViewFrustum().CreateProjectionMatrix(REFRACTION_MEASURES.x, REFRACTION_MEASURES.y),
            cameraComponent->GetPosition(),
            glm::vec4(0.0f, -1.0f, 0.0f, WATER_LEVEL + WATER_CLIP_PLANE_OFFSET),
            { REFRACTION_MEASURES.x, REFRACTION_MEASURES.y },
            glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane())
        };

        // SkyBox
        m_refractionSkyBoxRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_refractionSkyBoxRenderer->Render(renderContext, child, userData);
        }

        m_refractionSkyBoxRenderer->PostRender(renderContext, userData);

        // Default
        m_refractionDefaultRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_refractionDefaultRenderer->Render(renderContext, child, userData);
        }

        m_refractionDefaultRenderer->PostRender(renderContext, userData);

        // NormalMapped
        m_refractionNormalMappedRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_refractionNormalMappedRenderer->Render(renderContext, child, userData);
        }

        m_refractionNormalMappedRenderer->PostRender(renderContext, userData);

        // Terrain
        m_refractionTerrainRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_refractionTerrainRenderer->Render(renderContext, child, userData);
        }

        m_refractionTerrainRenderer->PostRender(renderContext, userData);
        
        // Terrain Normal Mapped
        m_refractionTerrainNormalMappedRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_refractionTerrainNormalMappedRenderer->Render(renderContext, child, userData);
        }

        m_refractionTerrainNormalMappedRenderer->PostRender(renderContext, userData);

        // Animation
        m_refractionAnimationRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_refractionAnimationRenderer->Render(renderContext, child, userData);
        }

        m_refractionAnimationRenderer->PostRender(renderContext, userData);
         
        // Animation Normal Mapped
        m_refractionAnimationNormalMappedRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_refractionAnimationNormalMappedRenderer->Render(renderContext, child, userData);
        }

        m_refractionAnimationNormalMappedRenderer->PostRender(renderContext, userData);

        refractionComponent->GetRenderPass()->End(renderContext.defaultCommandBuffer);
    }

    void RenderScene(RenderContext& renderContext, const std::shared_ptr<ISceneNode<SceneNodeFlags> >& root)
    {
        m_sunRenderer->BeforeRender(renderContext);

        m_defaultRenderPass->Begin(renderContext.defaultFrameBuffer, renderContext.defaultCommandBuffer, { { 0, 0 }, renderContext.fullExtent });

        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        NormalRenderContextUserData userData{
            cameraComponent->LookAt(),
            cameraComponent->GetViewFrustum().CreateProjectionMatrix(renderContext.fullExtent.width, renderContext.fullExtent.height),
            cameraComponent->GetPosition(),
            DEFAULT_CLIP_PLANE,
            renderContext.fullExtent,
            glm::vec2(cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane())
        };

        // SkyBox
        m_skyboxRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_skyboxRenderer->Render(renderContext, child, userData);
        }

        m_skyboxRenderer->PostRender(renderContext, userData);

        // Default
        m_defaultRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_defaultRenderer->Render(renderContext, child, userData);
        }

        m_defaultRenderer->PostRender(renderContext, userData);

        // NormalMapped
        m_normalMappedRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_normalMappedRenderer->Render(renderContext, child, userData);
        }

        m_normalMappedRenderer->PostRender(renderContext, userData);

        // Terrain
        m_terrainRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_terrainRenderer->Render(renderContext, child, userData);
        }

        m_terrainRenderer->PostRender(renderContext, userData);

        // Terrain Normal Mapped
        m_terrainNormalRendererRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_terrainNormalRendererRenderer->Render(renderContext, child, userData);
        }

        m_terrainNormalRendererRenderer->PostRender(renderContext, userData);

        // Animation
        m_animationRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_animationRenderer->Render(renderContext, child, userData);
        }

        m_animationRenderer->PostRender(renderContext, userData);

        // Animation Normal Mapped
        m_animationNormalMappedRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_animationNormalMappedRenderer->Render(renderContext, child, userData);
        }

        m_animationNormalMappedRenderer->PostRender(renderContext, userData);

        // Water
        m_waterRenderer->PreRender(renderContext, userData);

        for (auto child : root->GetChildren()) {
            m_waterRenderer->Render(renderContext, child, userData);
        }

        m_waterRenderer->PostRender(renderContext, userData);

        // Sun
        m_sunRenderer->PreRender(renderContext);

        for (auto child : root->GetChildren()) {
            m_sunRenderer->Render(renderContext, child);
        }

        m_sunRenderer->PostRender(renderContext);

        m_sunRenderer->AfterRender(renderContext);

        // LensFlare
        m_lensFlareRenderer->PreRender(renderContext);

        for (auto child : root->GetChildren()) {
            m_lensFlareRenderer->Render(renderContext, child);
        }

        m_lensFlareRenderer->PostRender(renderContext);

        // Fonts
        m_fontRenderer->PreRender(renderContext);

        for (auto child : root->GetChildren()) {
            m_fontRenderer->Render(renderContext, child);
        }

        m_fontRenderer->PostRender(renderContext);

        m_defaultRenderPass->End(renderContext.defaultCommandBuffer);
    }

private:
    std::shared_ptr<RenderPass> m_defaultRenderPass;

private:
    // Default Render
    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_skyboxRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_defaultRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_normalMappedRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_terrainRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_terrainNormalRendererRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_animationRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_animationNormalMappedRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_waterRenderer;

    std::unique_ptr<IRenderer<DefaultRenderContextUserData> > m_fontRenderer;

    std::unique_ptr<IRenderer<DefaultRenderContextUserData> > m_sunRenderer;

    std::unique_ptr<IRenderer<DefaultRenderContextUserData> > m_lensFlareRenderer;
    // Debug
    std::unique_ptr<IRenderer<DefaultRenderContextUserData> > m_shadowMapDebugRenderer;

    std::unique_ptr<IRenderer<DefaultRenderContextUserData> > m_textureDebugRenderer;

    // Shadows
    std::unique_ptr<IRenderer<ShadowsRenderContextUserData> > m_defaultShadowsRenderer;

    std::unique_ptr<IRenderer<ShadowsRenderContextUserData> > m_normalMappedShadowsRenderer;

    std::unique_ptr<IRenderer<ShadowsRenderContextUserData> > m_terrainShadowsRenderer;
    
    std::unique_ptr<IRenderer<ShadowsRenderContextUserData> > m_terrainNormalMappedShadowsRenderer;

    std::unique_ptr<IRenderer<ShadowsRenderContextUserData> > m_animationShadowsRenderer;

    std::unique_ptr<IRenderer<ShadowsRenderContextUserData> > m_animationNormalMappedShadowsRenderer;

    // Reflection
    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionSkyBoxRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionDefaultRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionNormalMappedRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionTerrainRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionTerrainNormalMappedRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionAnimationRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_reflectionAnimationNormalMappedRenderer;

    // Refraction
    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_refractionSkyBoxRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_refractionDefaultRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_refractionNormalMappedRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_refractionTerrainRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_refractionTerrainNormalMappedRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_refractionAnimationRenderer;

    std::unique_ptr<IRenderer<NormalRenderContextUserData> > m_refractionAnimationNormalMappedRenderer;
};

#endif