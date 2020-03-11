#ifndef __WATER_H__
#define __WATER_H__

#include "General.h"

static const float WATER_TILE_SIZE = 60.0f;

class WaterTile {
private:
    float m_height;

    float m_x, m_z;

public:
    explicit WaterTile(const float centerX, const float centerZ, const float height)
        : m_x(centerX)
        , m_z(centerZ)
        , m_height(height)
    {
    }

    virtual ~WaterTile() = default;

public:
    float GetHeight() const
    {
        return m_height;
    }

    void SetHeight(const float height)
    {
        m_height = height;
    }

    float GetX() const
    {
        return m_x;
    }

    void SetX(const float x)
    {
        m_x = x;
    }

    float GetZ() const
    {
        return m_z;
    }

    float SetZ(const float z)
    {
        m_z = z;
    }
};

class IWaterOffscreenRenderPassComponent {
public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<RenderPass> GetRenderPass() const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual std::shared_ptr<IImageBuffer> GetImageBuffer() const = 0;

    virtual VkFramebuffer GetFrameBuffer() const = 0;

public:
    virtual ~IWaterOffscreenRenderPassComponent() = default;
};

class WaterOffscreenRenderPassComponent : public IWaterOffscreenRenderPassComponent {
public:
    static const inline VkFormat COLOR_FORMAT = VK_FORMAT_B8G8R8A8_UNORM;

    static const inline VkFormat DEPTH_FORMAT = VK_FORMAT_D32_SFLOAT;

public:
    WaterOffscreenRenderPassComponent(const uint32_t w, const uint32_t h)
        : m_width(w)
        , m_height(h)
        , m_renderPass(nullptr)
        , m_imageBuffer(nullptr)
        , m_frameBuffer(nullptr)
    {
    }

    ~WaterOffscreenRenderPassComponent() = default;

public:
    void Init() override
    {
        InitRenderPass();
        InitBuffers();
    }

    void ShutDown() override
    {
        ShutDownBuffers();
        ShutDownRenderPass();
    }

    std::shared_ptr<RenderPass> GetRenderPass() const override
    {
        return m_renderPass;
    }

    VkExtent2D GetExtent() const override
    {
        return VkExtent2D{ m_width, m_height };
    }

    std::shared_ptr<IImageBuffer> GetImageBuffer() const override
    {
        return m_imageBuffer;
    }

    VkFramebuffer GetFrameBuffer() const override
    {
        return m_frameBuffer;
    }

private:
    void InitBuffers()
    {
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();
        auto device = DeviceProvider::GetInstance().GetDevice();

        m_imageBuffer = std::make_shared<ColorImageBuffer>(*allocator);
        m_imageBuffer->Create(ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, COLOR_FORMAT, 0, false, VK_IMAGE_VIEW_TYPE_2D });
        m_imageBuffer->CreateSampler();

        m_frameBuffer = VkUtils::CreateFrameBuffer(*device, *m_renderPass, { m_imageBuffer->GetImageView() }, GetExtent());
    }

    void ShutDownBuffers()
    {
        auto device = DeviceProvider::GetInstance().GetDevice();

        vkDeviceWaitIdle(*device);

        vkDestroyFramebuffer(*device, m_frameBuffer, nullptr);

        m_imageBuffer->Destroy();
    }

    void InitRenderPass()
    {
        auto device = DeviceProvider::GetInstance().GetDevice();

        std::vector<VkSubpassDependency> dependencies{ 2 };
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        m_renderPass = std::make_shared<RenderPass>(*device);
        m_renderPass->AddColorAttachment(COLOR_FORMAT, { 0.5f, 0.5f, 0.5f, 1.0f });
        m_renderPass->AddDepthAttachment(DEPTH_FORMAT);
        m_renderPass->AddSubpass({ 0, 1 });
        m_renderPass->AddSubpassDependency(dependencies);
        m_renderPass->Create();
    }

    void ShutDownRenderPass()
    {
        m_renderPass->Destroy();
    }

private:
    const uint32_t m_width;

    const uint32_t m_height;

    std::shared_ptr<RenderPass> m_renderPass;

    std::shared_ptr<ColorImageBuffer> m_imageBuffer;

    VkFramebuffer m_frameBuffer;
};

class IWaterComponent {
public:
    virtual std::shared_ptr<Image> GetDudvMapImage() const = 0;

    virtual std::shared_ptr<ImageBuffer> GetDudvMapImageBuffer() const = 0;

    virtual std::shared_ptr<Image> GetDudvMapNormalImage() const = 0;

    virtual std::shared_ptr<ImageBuffer> GetDudvMapNormalImageBuffer() const = 0;

    virtual std::shared_ptr<IModel> GetModel() const = 0;

public:
    virtual ~IWaterComponent() = default;
};

class WaterComponent : public IWaterComponent {
public:
    WaterComponent()
    {
    }

    ~WaterComponent() = default;

public:
    std::shared_ptr<Image> GetDudvMapImage() const override
    {
        return m_dudvMapImage;
    }

    std::shared_ptr<ImageBuffer> GetDudvMapImageBuffer() const override
    {
        return m_dudvMapImageBuffer;
    }

    std::shared_ptr<Image> GetDudvMapNormalImage() const override
    {
        return m_dudvMapNormalImage;
    }

    std::shared_ptr<ImageBuffer> GetDudvMapNormalImageBuffer() const override
    {
        return m_dudvMapNormalImageBuffer;
    }

    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

private:
    std::shared_ptr<Image> m_dudvMapImage;

    std::shared_ptr<ImageBuffer> m_dudvMapImageBuffer;

    std::shared_ptr<Image> m_dudvMapNormalImage;

    std::shared_ptr<ImageBuffer> m_dudvMapNormalImageBuffer;

    std::shared_ptr<IModel> m_model;
};

class WaterOffscreenComponentsFactory {
public:
    std::unique_ptr<IWaterOffscreenRenderPassComponent> Create(const uint32_t w, const uint32_t h) const
    {
        return std::make_unique<WaterOffscreenRenderPassComponent>(w, h);
    }
};

class WaterComponentFactory {
public:
    std::unique_ptr<IWaterComponent> Create() const
    {
        auto waterComponent = std::make_unique<WaterComponent>();
        // TODO
        return waterComponent;
    }
};

#endif