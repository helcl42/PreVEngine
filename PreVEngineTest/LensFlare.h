#ifndef __LENS_FLARE_H__
#define __LENS_FLARE_H__

#include "General.h"

class LensFlareItem {
public:
    LensFlareItem(const std::shared_ptr<Image>& image, const std::shared_ptr<ImageBuffer>& imageBuffer, const float scale)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_scale(scale)
        , m_screenSpacePosition({ 0.0f, 0.0f })
    {
    }

    ~LensFlareItem() = default;

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

class LensFlareMesh : public IMesh {
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

    virtual const std::vector<std::shared_ptr<LensFlareItem> >& GetFlares() const = 0;

    virtual std::shared_ptr<IModel> GetModel() = 0;

    virtual float GetBrightness() const = 0;

public:
    virtual ~ILensFlareComponent() = default;
};

class LensFlareComponent : public ILensFlareComponent {
public:
    explicit LensFlareComponent(const std::vector<std::shared_ptr<LensFlareItem> >& flares, float spacing, const std::shared_ptr<IModel>& model)
        : m_flares(flares)
        , m_spacing(spacing)
        , m_model(model)
        , m_brightness(0.0f)
    {
    }

    virtual ~LensFlareComponent() = default;

public:    
    void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) override
    {
        glm::vec2 sunPositionInScreenSpace;
        if (ConvertWorldSpaceToScreenSpaceCoord(eyePosition + sunPosition, projectionMatrix, viewMatrix, sunPositionInScreenSpace)) {
            const glm::vec2 SCREEN_CENTER{ 0.0f, 0.0f };
            glm::vec2 sunToCenter{ SCREEN_CENTER - sunPositionInScreenSpace };
            m_brightness = 1.0f - (glm::length(sunToCenter) / 1.4f);

            if (m_brightness > 0) {
                UpdateFlareTexrures(sunToCenter, sunPositionInScreenSpace);
            }

            m_brightness = glm::clamp(m_brightness, 0.0f, 0.5f);
        }
    }

    const std::vector<std::shared_ptr<LensFlareItem> >& GetFlares() const override
    {
        return m_flares;
    }

    std::shared_ptr<IModel> GetModel() override
    {
        return m_model;
    }

    float GetBrightness() const override
    {
        return m_brightness;
    }

private:
    bool ConvertWorldSpaceToScreenSpaceCoord(const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, glm::vec2& convertedSceenPosition)
    {
        glm::vec4 coord{ worldPosition, 1.0f };
        coord = viewMatrix * coord;
        coord = projectionMatrix * coord;
        if (coord.w <= 0.0f) {
            return false;
        }
        convertedSceenPosition = glm::vec2(coord.x / coord.w, coord.y / coord.w);
        return true;
    }

    void UpdateFlareTexrures(const glm::vec2& sunToCenterDirection, const glm::vec2& sunPositionInScreenSpace)
    {
        for (unsigned int i = 0; i < m_flares.size(); i++) {
            glm::vec2 direction = sunToCenterDirection * (i * m_spacing);
            glm::vec2 flarePosition = sunPositionInScreenSpace + direction;
            m_flares[i]->SetScreenSpacePosition(flarePosition);
        }
    }

private:
    std::vector<std::shared_ptr<LensFlareItem> > m_flares;

    float m_spacing;

    std::shared_ptr<IModel> m_model;

    float m_brightness;
};

class LensFlareComponentFactory {
public:
    std::unique_ptr<ILensFlareComponent> Create() const
    {
        const float spacing{ 0.2f };
        const std::vector<FlareCreateInfo> flareCreateInfos = {
            { AssetManager::Instance().GetAssetPath("LensFlares/tex2.png"), 0.2f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex3.png"), 0.12f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex4.png"), 0.46f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex5.png"), 0.14f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex6.png"), 1.0f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex7.png"), 0.1f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex8.png"), 1.2f },
            { AssetManager::Instance().GetAssetPath("LensFlares/tex9.png"), 0.24f },
        };

        auto allocator = AllocatorProvider::Instance().GetAllocator();

        std::vector<std::shared_ptr<LensFlareItem>> flares{};
        for (const auto& flareCreateInfo : flareCreateInfos) {
            flares.emplace_back(CreateFlare(*allocator, flareCreateInfo.path, flareCreateInfo.scale));
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
        auto mesh = std::make_unique<LensFlareMesh>();
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertices(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GerIndices().data(), static_cast<uint32_t>(mesh->GerIndices().size()));

        return std::make_unique<Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    std::unique_ptr<LensFlareItem> CreateFlare(Allocator& allocator, const std::string& filePath, const float scale) const
    {
        ImageFactory imageFactory{};
        auto image = imageFactory.CreateImage(filePath);
        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ { image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
        return std::make_unique<LensFlareItem>(std::move(image), std::move(imageBuffer), scale);
    }
};

#endif