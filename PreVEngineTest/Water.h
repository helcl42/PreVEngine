#ifndef __WATER_H__
#define __WATER_H__

#include "General.h"

static const float WATER_TILE_SIZE{ 20.0f };
static const float WATER_LEVEL{ -12.0f };
static const float WATER_CLIP_PLANE_OFFSET{ 0.08f };
static const float WATER_WAVE_SPEED{ 0.03f };
static const glm::uvec2 REFRACTION_MEASURES{ 640, 360 };
static const glm::uvec2 REFLECTION_MEASURES{ 320, 180 };


class WaterTileMesh : public IMesh {
public:
    const VertexLayout& GetVertexLayout() const override
    {
        return vertexLayout;
    }

    const void* GetVertices() const override
    {
        return (const void*)vertices.data();
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return indices;
    }

    bool HasIndices() const override
    {
        return indices.size() > 0;
    }

private:
    static const inline VertexLayout vertexLayout{ { VertexLayoutComponent::VEC3 } };

    static const inline std::vector<glm::vec3> vertices = {
        { 1.0f, 0.0f, 1.0f },
        { -1.0f, 0.0f, 1.0f },
        { -1.0f, 0.0f, -1.0f },
        { 1.0f, 0.0f, -1.0f }
    };

    static const inline std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };
};

class IWaterOffscreenRenderPassComponent {
public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<RenderPass> GetRenderPass() const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual std::shared_ptr<IImageBuffer> GetColorImageBuffer() const = 0;

    virtual std::shared_ptr<IImageBuffer> GetDepthImageBuffer() const = 0;

    virtual VkFramebuffer GetFrameBuffer() const = 0;

public:
    virtual ~IWaterOffscreenRenderPassComponent() = default;
};

class WaterOffScreenRenderPassComponent : public IWaterOffscreenRenderPassComponent {
public:
    static const inline VkFormat COLOR_FORMAT = VK_FORMAT_B8G8R8A8_UNORM;

    static const inline VkFormat DEPTH_FORMAT = VK_FORMAT_D32_SFLOAT;

public:
    WaterOffScreenRenderPassComponent(const uint32_t w, const uint32_t h)
        : m_width(w)
        , m_height(h)
        , m_renderPass(nullptr)
        , m_imageBuffer(nullptr)
        , m_depthBuffer(nullptr)
        , m_frameBuffer(nullptr)
    {
    }

    ~WaterOffScreenRenderPassComponent() = default;

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

    std::shared_ptr<IImageBuffer> GetColorImageBuffer() const override
    {
        return m_imageBuffer;
    }

    std::shared_ptr<IImageBuffer> GetDepthImageBuffer() const override
    {
        return m_depthBuffer;
    }

    VkFramebuffer GetFrameBuffer() const override
    {
        return m_frameBuffer;
    }

private:
    void InitBuffers()
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();
        auto device = DeviceProvider::Instance().GetDevice();

        m_imageBuffer = std::make_shared<ColorImageBuffer>(*allocator);
        m_imageBuffer->Create(ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, COLOR_FORMAT, 0, false, VK_IMAGE_VIEW_TYPE_2D });
        m_imageBuffer->CreateSampler();

        m_depthBuffer = std::make_shared<DepthImageBuffer>(*allocator);
        m_depthBuffer->Create(ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, DEPTH_FORMAT, 0, false, VK_IMAGE_VIEW_TYPE_2D });
        m_depthBuffer->CreateSampler();

        m_frameBuffer = VkUtils::CreateFrameBuffer(*device, *m_renderPass, { m_imageBuffer->GetImageView(), m_depthBuffer->GetImageView() }, GetExtent());
    }

    void ShutDownBuffers()
    {
        auto device = DeviceProvider::Instance().GetDevice();

        vkDeviceWaitIdle(*device);

        vkDestroyFramebuffer(*device, m_frameBuffer, nullptr);

        m_depthBuffer->Destroy();
        m_imageBuffer->Destroy();
    }

    void InitRenderPass()
    {
        auto device = DeviceProvider::Instance().GetDevice();

        std::vector<VkSubpassDependency> dependencies{ 2 };
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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
        m_renderPass->AddColorAttachment(COLOR_FORMAT, { 0.5f, 0.5f, 0.5f, 1.0f }, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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

    std::shared_ptr<DepthImageBuffer> m_depthBuffer;

    VkFramebuffer m_frameBuffer;
};

class IWaterComponent : public IBasicRenderComponent {
public:
    virtual std::shared_ptr<IMaterial> GetMaterial() const = 0;

    virtual void Update(float deltaTime) = 0;

    virtual float GetMoveFactor() const = 0;

    virtual const glm::vec3& GetPosition() const = 0;

    virtual int GetGridX() const = 0;

    virtual int GetGridZ() const = 0;

public:
    virtual ~IWaterComponent() = default;
};

class WaterComponent : public IWaterComponent {
public:
    WaterComponent(const int gridX, const int gridZ, const std::shared_ptr<IMaterial>& material, const std::shared_ptr<IModel>& model)
        : m_gridX(gridX)
        , m_gridZ(gridZ)
        , m_position(glm::vec3(gridX * 2 * WATER_TILE_SIZE + WATER_TILE_SIZE, WATER_LEVEL, gridZ * 2 * WATER_TILE_SIZE + WATER_TILE_SIZE))
        , m_material(material)
        , m_model(model)
        , m_moveFactor(0.0f)
    {
    }

    ~WaterComponent() = default;

public:
    std::shared_ptr<IMaterial> GetMaterial() const override
    {
        return m_material;
    }

    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

    void Update(float deltaTime) override
    {
        m_moveFactor += WATER_WAVE_SPEED * deltaTime;
        m_moveFactor = fmodf(m_moveFactor, 1.0f);
    }

    float GetMoveFactor() const override
    {
        return m_moveFactor;
    }

    const glm::vec3& GetPosition() const override
    {
        return m_position;
    }

    int GetGridX() const override
    {
        return m_gridX;
    }

    int GetGridZ() const override
    {
        return m_gridZ;
    }

private:
    const int m_gridX;

    const int m_gridZ;

    const glm::vec3 m_position;

    std::shared_ptr<IMaterial> m_material;

    std::shared_ptr<IModel> m_model;

    float m_moveFactor;
};

class WaterComponentFactory {
public:
    std::unique_ptr<IWaterComponent> Create(const int x, const int z) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        const glm::vec4 waterColor{ 0.0f, 0.3f, 0.5f, 1.0f };
        const std::string dudvMapPath{ AssetManager::Instance().GetAssetPath("Textures/waterDUDV.png") };
        const std::string normalMapPath{ AssetManager::Instance().GetAssetPath("Textures/matchingNormalMap.png") };

        auto material = CreateMaterial(*allocator, waterColor, dudvMapPath, normalMapPath, 1.0f, 0.4f);
        auto model = CreateModel(*allocator);

        return std::make_unique<WaterComponent>(x, z, std::move(material), std::move(model));
    }

    std::unique_ptr<IWaterOffscreenRenderPassComponent> CreateOffScreenComponent(const uint32_t w, const uint32_t h) const
    {
        return std::make_unique<WaterOffScreenRenderPassComponent>(w, h);
    }

private:
    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const glm::vec4& color, const std::string& textureFilename, const std::string& normalTextureFilename, const float shineDamper, const float reflectivity) const
    {
        ImageFactory imageFactory;
        auto image = imageFactory.CreateImage(textureFilename);
        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ { image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)image->GetBuffer() });

        auto normalImage = imageFactory.CreateImage(textureFilename);
        auto normalImageBuffer = std::make_unique<ImageBuffer>(allocator);
        normalImageBuffer->Create(ImageBufferCreateInfo{ { normalImage->GetWidth(), normalImage->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)normalImage->GetBuffer() });

        return std::make_unique<Material>(color, std::move(image), std::move(imageBuffer), std::move(normalImage), std::move(normalImageBuffer), shineDamper, reflectivity);
    }

    std::unique_ptr<IModel> CreateModel(Allocator& allocator) const
    {
        auto mesh = std::make_unique<WaterTileMesh>();
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertices(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GerIndices().data(), static_cast<uint32_t>(mesh->GerIndices().size()));

        return std::make_unique<Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }
};

#endif