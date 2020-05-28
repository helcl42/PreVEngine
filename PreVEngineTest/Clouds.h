#ifndef __CLOUDS_H__
#define __CLOUDS_H__

#include "General.h"
#include "Pipeline.h"

class WatherFactory {
public:
    std::unique_ptr<IImageBuffer> Create(const uint32_t width, const uint32_t height) const
    {
        struct Uniforms {
            alignas(16) glm::vec4 textureSize;
            alignas(16) glm::vec4 seed;
            alignas(16) float perlinAmplitude;
            float perlinFrequency;
            float perlinScale;
            int perlinOctaves;
        };

        auto device = DeviceProvider::Instance().GetDevice();
        auto computeQueue = ComputeProvider::Instance().GetQueue();
        auto computeAllocator = ComputeProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory{};
        auto shader = shaderFactory.CreateShaderFromFiles<WeatherComputeShader>(*device, { { VK_SHADER_STAGE_COMPUTE_BIT, AssetManager::Instance().GetAssetPath("Shaders/weather_comp.spv") } });

        auto pipeline = std::make_unique<WeatherComputePipeline>(*device, *shader);
        pipeline->Init();

        auto uniformsPool = std::make_unique<UBOPool<Uniforms> >(*computeAllocator);
        uniformsPool->AdjustCapactity(3, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

        auto commandPool = computeQueue->CreateCommandPool();
        auto commandBuffer = VkUtils::CreateCommandBuffer(*device, commandPool);

        auto fence = VkUtils::CreateFence(*device);

        auto weatherImageBuffer = std::make_unique<ImageStorageBuffer>(*computeAllocator);
        weatherImageBuffer->Create(ImageBufferCreateInfo{ VkExtent2D{ width, height }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE });

        VKERRCHECK(vkQueueWaitIdle(*computeQueue));

        VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &cmdBufBeginInfo));

        Uniforms uniforms{};
        uniforms.textureSize = glm::vec4(width, height, 0, 0);
        uniforms.seed = glm::vec4(10, 20, 0, 0);
        uniforms.perlinAmplitude = 0.5f;
        uniforms.perlinFrequency = 0.8f;
        uniforms.perlinScale = 100.0f;
        uniforms.perlinOctaves = 4;

        auto ubo = uniformsPool->GetNext();
        ubo->Update(&uniforms);

        shader->Bind("uboCS", *ubo);
        shader->Bind("outWeatherTexture", *weatherImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
        const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

        vkCmdDispatch(commandBuffer, 128, 128, 1);

        VKERRCHECK(vkEndCommandBuffer(commandBuffer));

        // Submit compute work
        vkResetFences(*device, 1, &fence);

        const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo computeSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
        computeSubmitInfo.commandBufferCount = 1;
        computeSubmitInfo.pCommandBuffers = &commandBuffer;
        VKERRCHECK(vkQueueSubmit(*computeQueue, 1, &computeSubmitInfo, fence));
        VKERRCHECK(vkWaitForFences(*device, 1, &fence, VK_TRUE, UINT64_MAX));

        VKERRCHECK(vkQueueWaitIdle(*computeQueue));

        vkDestroyFence(*device, fence, nullptr);
        vkDestroyCommandPool(*device, commandPool, nullptr);

        computeAllocator->TransitionImageLayout(weatherImageBuffer->GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, weatherImageBuffer->GetMipLevels());

        pipeline->ShutDown();

        shader->ShutDown();

        return weatherImageBuffer;
    }
};

class WatherNoiseFactory {
public:
    std::unique_ptr<IImageBuffer> CreatePerlinWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto computeQueue = ComputeProvider::Instance().GetQueue();
        auto computeAllocator = ComputeProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory{};
        auto shader = shaderFactory.CreateShaderFromFiles<PerlinWorleyComputeShader>(*device, { { VK_SHADER_STAGE_COMPUTE_BIT, AssetManager::Instance().GetAssetPath("Shaders/worley_noise_3d_comp.spv") } });

        auto pipeline = std::make_unique<PerlinWorleyComputePipeline>(*device, *shader);
        pipeline->Init();

        auto commandPool = computeQueue->CreateCommandPool();
        auto commandBuffer = VkUtils::CreateCommandBuffer(*device, commandPool);

        auto fence = VkUtils::CreateFence(*device);

        auto perlinNoise3DImageBuffer = std::make_unique<ImageStorageBuffer>(*computeAllocator);
        perlinNoise3DImageBuffer->Create(ImageBufferCreateInfo{ VkExtent3D{ width, height, depth }, VK_IMAGE_TYPE_3D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_3D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE });

        VKERRCHECK(vkQueueWaitIdle(*computeQueue));

        VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &cmdBufBeginInfo));

        shader->Bind("outVolumeTexture", *perlinNoise3DImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
        const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

        vkCmdDispatch(commandBuffer, 32, 32, 32);

        VKERRCHECK(vkEndCommandBuffer(commandBuffer));

        // Submit compute work
        vkResetFences(*device, 1, &fence);

        const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo computeSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
        computeSubmitInfo.commandBufferCount = 1;
        computeSubmitInfo.pCommandBuffers = &commandBuffer;
        VKERRCHECK(vkQueueSubmit(*computeQueue, 1, &computeSubmitInfo, fence));
        VKERRCHECK(vkWaitForFences(*device, 1, &fence, VK_TRUE, UINT64_MAX));

        VKERRCHECK(vkQueueWaitIdle(*computeQueue));

        vkDestroyFence(*device, fence, nullptr);
        vkDestroyCommandPool(*device, commandPool, nullptr);

        computeAllocator->TransitionImageLayout(perlinNoise3DImageBuffer->GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, perlinNoise3DImageBuffer->GetMipLevels());

        pipeline->ShutDown();

        shader->ShutDown();

        return perlinNoise3DImageBuffer;
    }

    std::unique_ptr<IImageBuffer> CreateWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto computeQueue = ComputeProvider::Instance().GetQueue();
        auto computeAllocator = ComputeProvider::Instance().GetAllocator();

        ShaderFactory shaderFactory{};
        auto shader = shaderFactory.CreateShaderFromFiles<PerlinWorleyComputeShader>(*device, { { VK_SHADER_STAGE_COMPUTE_BIT, AssetManager::Instance().GetAssetPath("Shaders/perlin_worley_noise_3d_comp.spv") } });

        auto pipeline = std::make_unique<WorleyComputePipeline>(*device, *shader);
        pipeline->Init();

        auto commandPool = computeQueue->CreateCommandPool();
        auto commandBuffer = VkUtils::CreateCommandBuffer(*device, commandPool);

        auto fence = VkUtils::CreateFence(*device);

        auto perlinNoise3DImageBuffer = std::make_unique<ImageStorageBuffer>(*computeAllocator);
        perlinNoise3DImageBuffer->Create(ImageBufferCreateInfo{ VkExtent3D{ width, height, depth }, VK_IMAGE_TYPE_3D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_3D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE });

        VKERRCHECK(vkQueueWaitIdle(*computeQueue));

        VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &cmdBufBeginInfo));

        shader->Bind("outVolumeTexture", *perlinNoise3DImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
        const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

        vkCmdDispatch(commandBuffer, 8, 8, 8);

        VKERRCHECK(vkEndCommandBuffer(commandBuffer));

        // Submit compute work
        vkResetFences(*device, 1, &fence);

        const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo computeSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
        computeSubmitInfo.commandBufferCount = 1;
        computeSubmitInfo.pCommandBuffers = &commandBuffer;
        VKERRCHECK(vkQueueSubmit(*computeQueue, 1, &computeSubmitInfo, fence));
        VKERRCHECK(vkWaitForFences(*device, 1, &fence, VK_TRUE, UINT64_MAX));

        VKERRCHECK(vkQueueWaitIdle(*computeQueue));

        vkDestroyFence(*device, fence, nullptr);
        vkDestroyCommandPool(*device, commandPool, nullptr);

        computeAllocator->TransitionImageLayout(perlinNoise3DImageBuffer->GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, perlinNoise3DImageBuffer->GetMipLevels());

        pipeline->ShutDown();

        shader->ShutDown();

        return perlinNoise3DImageBuffer;
    }
};

class ICloudsComponent {
public:
    virtual void Update(float deltaTime) = 0;

public:
    virtual ~ICloudsComponent() = default;
};

class CloudsComponentFactory;

class CloudsComponent : public ICloudsComponent {
public:    
    void Update(float deltaTime) override
    {
    }

private:
    friend class CloudsComponentFactory;

private:
    std::unique_ptr<IImageBuffer> m_weatherImageBuffer;

    std::unique_ptr<IImageBuffer> m_perlinWorleyNoiseImageBuffer;

    std::unique_ptr<IImageBuffer> m_worleyNoiseImageBuffer;
};

class CloudsComponentFactory {
public:
    std::unique_ptr<CloudsComponent> Create() const
    {
        WatherFactory weatherFactory{};
        auto weather = weatherFactory.Create(1024, 1024);

        WatherNoiseFactory weatherNoiseFactory{};
        auto worleyNoise = weatherNoiseFactory.CreateWorleyNoise(32, 32, 32);
        auto perlinWorleyNoise = weatherNoiseFactory.CreatePerlinWorleyNoise(128, 128, 128);

        auto clouds = std::make_unique<CloudsComponent>();
        clouds->m_weatherImageBuffer = std::move(weather);
        clouds->m_worleyNoiseImageBuffer = std::move(worleyNoise);
        clouds->m_perlinWorleyNoiseImageBuffer = std::move(perlinWorleyNoise);
        return clouds;
    }
};

#endif // !__CLOUDS_H__
