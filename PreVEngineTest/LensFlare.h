#ifndef __LENS_FLARE_H__
#define __LENS_FLARE_H__

#include "General.h"

class Flare {
public:
    Flare(const std::shared_ptr<Image>& image, const std::shared_ptr<ImageBuffer>& imageBuffer, const float scale)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_scale(scale)
        , m_screenSpacePosition({ 0.0f, 0.0f })
    {
    }

    ~Flare() = default;

public:
    std::shared_ptr<Image> GetImage() const
    {
        return m_image;
    }

    std::shared_ptr<ImageBuffer> GetImageBuffer() const
    {
        return m_imageBuffer;
    }

    float GetScale() const
    {
        return m_scale;
    }

    const glm::vec2& GetScreenSpacePosition() const
    {
        return m_screenSpacePosition;
    }

    void SetScreenSpacePosition(const glm::vec2& position)
    {
        m_screenSpacePosition = position;
    }

private:
    std::shared_ptr<Image> m_image;

    std::shared_ptr<ImageBuffer> m_imageBuffer;

    float m_scale;

    glm::vec2 m_screenSpacePosition;
};

class QuadMesh2D : public IMesh {
public:
    const VertexLayout& GetVertexLayout() const override
    {
        return vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return (const void*)vertices.data();
    }

    std::vector<glm::vec3> GetVertices() const override
    {
        std::vector<glm::vec3> verts{ vertices.size() };
        for (auto i = 0; i < vertices.size(); i++) {
            verts[i] = glm::vec3(vertices[i], 0.0f);
        }
        return verts;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return indices;
    }

    bool HasIndices() const override
    {
        return indices.size() > 0;
    }

private:
    static const inline VertexLayout vertexLayout{ { VertexLayoutComponent::VEC2 } };

    static const inline std::vector<glm::vec2> vertices = {
        { 0.5f, 0.5f },
        { -0.5f, 0.5f },
        { -0.5f, -0.5f },
        { 0.5f, -0.5f }
    };

    static const inline std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };
};

class ILensFlareComponent {
public:
    virtual void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) = 0;

    virtual const std::vector<std::shared_ptr<Flare> >& GetFlares() const = 0;

    virtual std::shared_ptr<IModel> GetModel() const = 0;

public:
    virtual ~ILensFlareComponent() = default;
};

class LensFlareComponent : public ILensFlareComponent {
public:
    explicit LensFlareComponent(const std::vector<std::shared_ptr<Flare> >& flares, float spacing, const std::shared_ptr<IModel>& model)
        : m_flares(flares)
        , m_spacing(spacing)
        , m_model(model)
    {
    }

    virtual ~LensFlareComponent() = default;

public:    
    void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) override
    {
        glm::vec2 sunPositionInScreenSpace;
        if (ConvertWorldSpaceToScreenSpaceCoord(eyePosition + sunPosition, projectionMatrix, viewMatrix, sunPositionInScreenSpace)) {
            const glm::vec2 screenCenter{ 0.0f, 0.0f };
            glm::vec2 sunToCenter{ screenCenter - sunPositionInScreenSpace };
            const float brightness = 1.0f - (glm::length(sunToCenter) / 1.4f);
            if (brightness > 0) {
                UpdateFlareTexrures(sunToCenter, sunPositionInScreenSpace);
            } else {
                UpdateFlareTexrures(glm::vec2(-100.0f), glm::vec2(-100.0f));
            }
        }
    }

    const std::vector<std::shared_ptr<Flare> >& GetFlares() const override
    {
        return m_flares;
    }

    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

private:
    bool ConvertWorldSpaceToScreenSpaceCoord(const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, glm::vec2& convertedSceenPosition)
    {
        const auto coord = projectionMatrix * viewMatrix * glm::vec4{ worldPosition, 1.0f };
        if (coord.w <= 0.0f) {
            return false;
        }
        convertedSceenPosition = glm::vec2(coord.x / coord.w, coord.y / coord.w);
        return true;
    }

    void UpdateFlareTexrures(const glm::vec2& sunToCenterDirection, const glm::vec2& sunPositionInScreenSpace)
    {
        for (unsigned int i = 0; i < m_flares.size(); i++) {
            const auto direction = sunToCenterDirection * ((i + 1) * m_spacing);
            const auto flarePosition = sunPositionInScreenSpace + direction;
            m_flares[i]->SetScreenSpacePosition(flarePosition);
        }
    }

private:
    std::vector<std::shared_ptr<Flare> > m_flares;

    float m_spacing;

    std::shared_ptr<IModel> m_model;
};

class LensFlareComponentFactory {
public:
    std::unique_ptr<ILensFlareComponent> Create() const
    {
        const float spacing{ 0.16f };
        const std::vector<FlareCreateInfo> flareCreateInfos = {
            { AssetManager::Instance().GetAssetPath("LensFlares/tex2.png"), 0.15f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex3.png"), 0.12f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex4.png"), 0.46f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex5.png"), 0.12f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex6.png"), 1.0f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex7.png"), 0.1f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex8.png"), 1.2f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex9.png"), 0.24f },
        };

        auto allocator = AllocatorProvider::Instance().GetAllocator();

        std::vector<std::shared_ptr<Flare> > flares{};
        for (const auto& flareCreateInfo : flareCreateInfos) {
            auto flare = CreateFlare(*allocator, flareCreateInfo.path, flareCreateInfo.scale);
            flare->SetScreenSpacePosition(glm::vec2(-100.0f, -100.0f));
            flares.emplace_back(std::move(flare));
        }

        auto model = CreateModel(*allocator);
        return std::make_unique<LensFlareComponent>(flares, spacing, std::move(model));
    }

private:
    struct FlareCreateInfo {
        std::string path;
        float scale;
    };

    std::unique_ptr<IModel> CreateModel(Allocator& allocator) const
    {
        auto mesh = std::make_unique<QuadMesh2D>();
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

        return std::make_unique<Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    std::unique_ptr<Flare> CreateFlare(Allocator& allocator, const std::string& filePath, const float scale) const
    {
        ImageFactory imageFactory{};
        auto image = imageFactory.CreateImage(filePath);
        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
        return std::make_unique<Flare>(std::move(image), std::move(imageBuffer), scale);
    }
};

class ISunComponent {
public:
    virtual void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) = 0;

    virtual std::shared_ptr<Flare> GetFlare() const = 0;

    virtual std::shared_ptr<IModel> GetModel() const = 0;

public:
    virtual ~ISunComponent() = default;
};

class SunComponent : public ISunComponent {
public:
    explicit SunComponent(const std::shared_ptr<Flare>& flare, const std::shared_ptr<IModel>& model)
        : m_flare(flare)
        , m_model(model)
    {
    }

    virtual ~SunComponent() = default;

public:
    void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) override
    {
        glm::vec2 sunPositionInScreenSpace;
        if (ConvertWorldSpaceToScreenSpaceCoord(eyePosition + sunPosition, projectionMatrix, viewMatrix, sunPositionInScreenSpace)) {
            const glm::vec2 screenCenter{ 0.0f, 0.0f };
            glm::vec2 sunToCenter{ screenCenter - sunPositionInScreenSpace };
            const float brightness = 1.0f - (glm::length(sunToCenter) / 1.4f);
            if (brightness > 0) {
                m_flare->SetScreenSpacePosition(sunPositionInScreenSpace);
            } else {
                m_flare->SetScreenSpacePosition(glm::vec2(-100.0f));
            }
        }
    }

    std::shared_ptr<Flare> GetFlare() const override
    {
        return m_flare;
    }

    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

private:
    bool ConvertWorldSpaceToScreenSpaceCoord(const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, glm::vec2& convertedSceenPosition)
    {
        const auto coord = projectionMatrix * viewMatrix * glm::vec4{ worldPosition, 1.0f };
        if (coord.w <= 0.0f) {
            return false;
        }
        convertedSceenPosition = glm::vec2(coord.x / coord.w, coord.y / coord.w);
        return true;
    }

private:
    std::shared_ptr<Flare> m_flare;

    std::shared_ptr<IModel> m_model;
};

class SunComponentFactory {
public:
    std::unique_ptr<ISunComponent> Create() const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto flare = CreateFlare(*allocator, AssetManager::Instance().GetAssetPath("Textures/sun.png"), 0.2f);
        flare->SetScreenSpacePosition(glm::vec2(-100.0f, -100.0f));

        auto model = CreateModel(*allocator);
        return std::make_unique<SunComponent>(std::move(flare), std::move(model));
    }

private:
    std::unique_ptr<IModel> CreateModel(Allocator& allocator) const
    {
        auto mesh = std::make_unique<QuadMesh2D>();
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

        return std::make_unique<Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    std::unique_ptr<Flare> CreateFlare(Allocator& allocator, const std::string& filePath, const float scale) const
    {
        ImageFactory imageFactory{};
        auto image = imageFactory.CreateImage(filePath);
        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
        return std::make_unique<Flare>(std::move(image), std::move(imageBuffer), scale);
    }
};


#endif