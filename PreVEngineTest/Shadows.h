#ifndef __SHADOWS_H__
#define __SHADOWS_H__

#include "General.h"

struct ShadowsCascade {
    VkFramebuffer frameBuffer;

    VkImageView imageView;

    float startSplitDepth;

    float endSplitDepth;

    glm::mat4 viewMatrix;

    glm::mat4 projectionMatrix;

    void Destroy(VkDevice device)
    {
        vkDestroyImageView(device, imageView, nullptr);
        vkDestroyFramebuffer(device, frameBuffer, nullptr);
    }

    glm::mat4 GetBiasedViewProjectionMatrix() const
    {
        static const glm::mat4 biasMat(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f);

        return biasMat * projectionMatrix * viewMatrix;
    }
};

class IShadowsComponent {
public:
    virtual void Init() = 0;

    virtual void Update(const glm::vec3& lightDirection, const float nearClippingPlane, const float farClippingPlane, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<RenderPass> GetRenderPass() const = 0;

    virtual const ShadowsCascade& GetCascade(const uint32_t cascadeIndex) const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual std::shared_ptr<IImageBuffer> GetImageBuffer() const = 0;

public:
    virtual ~IShadowsComponent() = default;
};

class ShadowsComponent : public IShadowsComponent {
public:
    static const inline VkFormat DEPTH_FORMAT = VK_FORMAT_D32_SFLOAT;

    static const inline uint32_t SHADOW_MAP_DIMENSIONS = 2048;

    static const inline VkFilter SHADOW_MAP_FILTER = VK_FILTER_LINEAR;

    static const inline uint32_t CASCADES_COUNT = 4;

    static const inline float CASCADES_SPLIT_LAMBDA = 0.86f;

private:
    std::shared_ptr<Allocator> m_allocator;

    std::shared_ptr<Device> m_device;

private:
    std::shared_ptr<RenderPass> m_renderPass;

    std::shared_ptr<DepthImageBuffer> m_depthBuffer;

    std::vector<ShadowsCascade> m_cascades;

public:
    ShadowsComponent() = default;

    virtual ~ShadowsComponent() = default;

private:
    void InitRenderPass()
    {
        auto device = DeviceProvider::Instance().GetDevice();

        std::vector<VkSubpassDependency> dependencies{ 2 };
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        m_renderPass = std::make_shared<RenderPass>(*device);
        m_renderPass->AddDepthAttachment(DEPTH_FORMAT);
        m_renderPass->AddSubpass({ 0 });
        m_renderPass->AddSubpassDependency(dependencies);
        m_renderPass->Create();
    }

    void ShutDownRenderPass()
    {
        m_renderPass->Destroy();
    }

    void InitCascades()
    {
        auto device = DeviceProvider::Instance().GetDevice();
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        m_depthBuffer = std::make_shared<DepthImageBuffer>(*allocator);
        m_depthBuffer->Create(ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, DEPTH_FORMAT, 0, false, true, VK_IMAGE_VIEW_TYPE_2D_ARRAY, CASCADES_COUNT });
        m_depthBuffer->CreateSampler();

        m_cascades.resize(CASCADES_COUNT);
        for (uint32_t i = 0; i < CASCADES_COUNT; i++) {
            auto& cascade = m_cascades.at(i);

            cascade.imageView = VkUtils::CreateImageView(*device, m_depthBuffer->GetImage(), m_depthBuffer->GetFormat(), VK_IMAGE_VIEW_TYPE_2D_ARRAY, m_depthBuffer->GetMipLevels(), VK_IMAGE_ASPECT_DEPTH_BIT, 1, i);
            cascade.frameBuffer = VkUtils::CreateFrameBuffer(*device, *m_renderPass, { cascade.imageView }, GetExtent());
        }
    }

    void ShutDownCascades()
    {
        auto device = DeviceProvider::Instance().GetDevice();

        vkDeviceWaitIdle(*device);

        for (uint32_t i = 0; i < CASCADES_COUNT; i++) {
            auto& cascade = m_cascades.at(i);
            cascade.Destroy(*device);
        }

        m_depthBuffer->Destroy();
    }

    std::vector<float> GenerateCaascadeSplits(const float nearClippingPlane, const float farClippingPlane) const
    {
        std::vector<float> cascadeSplits(CASCADES_COUNT);

        const float clipRange = farClippingPlane - nearClippingPlane;

        const float minZ = nearClippingPlane;
        const float maxZ = nearClippingPlane + clipRange;

        const float range = maxZ - minZ;
        const float ratio = maxZ / minZ;

        // Calculate split depths based on view camera furstum
        for (uint32_t i = 0; i < CASCADES_COUNT; i++) {
            float p = (i + 1) / static_cast<float>(CASCADES_COUNT);
            float log = minZ * powf(ratio, p);
            float uniform = minZ + range * p;
            float d = CASCADES_SPLIT_LAMBDA * (log - uniform) + uniform;
            cascadeSplits[i] = (d - nearClippingPlane) / clipRange;
        }

        return cascadeSplits;
    }

    std::vector<glm::vec3> GenerateFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform, const float splitDistance, const float lastSplitDistance) const
    {
        auto frustumCorners = MathUtil::GetFrustumCorners(inverseWorldToClipSpaceTransform);

        for (uint32_t i = 0; i < 4; i++) {
            glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
            frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDistance);
            frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDistance);
        }

        return frustumCorners;
    }

    glm::vec3 CalculateFrustumCenter(const std::vector<glm::vec3>& frustumCorners) const
    {
        glm::vec3 frustumCenter{ 0.0f };
        for (uint32_t i = 0; i < 8; i++) {
            frustumCenter += frustumCorners[i];
        }
        frustumCenter /= 8.0f;
        return frustumCenter;
    }

    float CalculateFrustumRadius(const std::vector<glm::vec3>& frustumCorners, const glm::vec3& frustumCenter) const
    {
        float radius = 0.0f;
        for (uint32_t i = 0; i < 8; i++) {
            float distance = glm::length(frustumCorners[i] - frustumCenter);
            radius = glm::max(radius, distance);
        }
        radius = std::ceil(radius * 16.0f) / 16.0f;
        return radius;
    }

    void UpdateCascade(const glm::vec3& lightDirection, const glm::mat4& inverseCameraTransform, const float nearClippingPlane, const float farClippingPlane, const float splitDistance, const float lastSplitDistance, ShadowsCascade& outCascade) const
    {
        const auto clipRange = farClippingPlane - nearClippingPlane;
        const auto frustumCorners = GenerateFrustumCorners(inverseCameraTransform, splitDistance, lastSplitDistance);
        const auto frustumCenter = CalculateFrustumCenter(frustumCorners);
        const auto radius = CalculateFrustumRadius(frustumCorners, frustumCenter);

        AABB aabb{ radius };

        const glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDirection * -aabb.minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 lightOrthoProjectionMatrix = glm::ortho(aabb.minExtents.x, aabb.maxExtents.x, aabb.minExtents.y, aabb.maxExtents.y, 0.0f, aabb.maxExtents.z - aabb.minExtents.z);

        outCascade.startSplitDepth = (nearClippingPlane + lastSplitDistance * clipRange) * -1.0f;
        outCascade.endSplitDepth = (nearClippingPlane + splitDistance * clipRange) * -1.0f;
        outCascade.viewMatrix = lightViewMatrix;
        outCascade.projectionMatrix = lightOrthoProjectionMatrix;
    }

public:
    void Init() override
    {
        InitRenderPass();
        InitCascades();
    }

    void Update(const glm::vec3& lightDirection, const float nearClippingPlane, const float farClippingPlane, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) override
    {
        const auto cascadeSplits = GenerateCaascadeSplits(nearClippingPlane, farClippingPlane);
        const glm::mat4 inverseWorldToClipSpaceTransform = glm::inverse(projectionMatrix * viewMatrix);

        float lastSplitDistance = 0.0;
        for (uint32_t i = 0; i < CASCADES_COUNT; i++) {
            const float splitDistance = cascadeSplits[i];

            // Calculate orthographic projection matrix for ith cascade
            UpdateCascade(lightDirection, inverseWorldToClipSpaceTransform, nearClippingPlane, farClippingPlane, splitDistance, lastSplitDistance, m_cascades[i]);

            lastSplitDistance = splitDistance;
        }
    }

    void ShutDown() override
    {
        ShutDownCascades();
        ShutDownRenderPass();
    }

    std::shared_ptr<RenderPass> GetRenderPass() const override
    {
        return m_renderPass;
    }

    const ShadowsCascade& GetCascade(const uint32_t cascadeIndex) const override
    {
        return m_cascades.at(cascadeIndex);
    }

    VkExtent2D GetExtent() const override
    {
        return { SHADOW_MAP_DIMENSIONS, SHADOW_MAP_DIMENSIONS };
    }

    std::shared_ptr<IImageBuffer> GetImageBuffer() const override
    {
        return m_depthBuffer;
    }
};

class ShadowsComponentFactory {
public:
    std::unique_ptr<IShadowsComponent> Create() const
    {
        return std::make_unique<ShadowsComponent>();
    }
};

#endif