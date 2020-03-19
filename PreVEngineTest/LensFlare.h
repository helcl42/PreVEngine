#ifndef __LENS_FLARE_H__
#define __LENS_FLARE_H__

#include "General.h"

class LensFlare {
public:
    LensFlare(const std::shared_ptr<Image>& image, const std::shared_ptr<ImageBuffer>& imageBuffer, const float scale)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_scale(scale)
        , m_screenSpacePosition({ 0.0f, 0.0f })
    {
    }

    ~LensFlare() = default;

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

class ILensFlareComponent {
public:
    virtual void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) = 0;

    virtual const std::vector<std::shared_ptr<LensFlare> >& GetFlares() const = 0;

public:
    virtual ~ILensFlareComponent() = default;
};

class LensFlareComponent : public ILensFlareComponent {
public:
    explicit LensFlareComponent(const std::vector<std::shared_ptr<LensFlare> >& flares, float spacing)
        : m_flares(flares)
        , m_spacing(spacing)
    {
    }

    virtual ~LensFlareComponent() = default;

public:    
    void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) override
    {
        glm::vec2 sunPositionInScreenSpace;
        if (ConvertWorldSpaceToScreenSpaceCoord(sunPositionInScreenSpace, eyePosition + sunPosition, projectionMatrix, viewMatrix)) {
            glm::vec2 sunToCenter{ SCREEN_CENTER - sunPositionInScreenSpace };
            const float brightness = 1.0f - (glm::length(sunToCenter) / 1.4f);
            if (brightness) {
                UpdateFlareTexrures(sunToCenter, sunPositionInScreenSpace);
            }
        }
    }

    const std::vector<std::shared_ptr<LensFlare> >& GetFlares() const override
    {
        return m_flares;
    }

private:
    bool ConvertWorldSpaceToScreenSpaceCoord(glm::vec2& convertedSceenPosition, const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
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
    static const inline glm::vec2 SCREEN_CENTER{ 0.0f, 0.0f };

private:
    std::vector<std::shared_ptr<LensFlare> > m_flares;

    float m_spacing;
};

class LensFlareComponentFactory {
public:
    std::unique_ptr<ILensFlareComponent> Create() const
    {
        const float spacing{ 0.16f };
        const std::vector<FlareCreateInfo> flareCreateInfos = {
            { AssetManager::Instance().GetAssetPath("LensFlares/tex6.png"), 1.0f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex4.png"), 0.46f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex2.png"), 0.2f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex7.png"), 0.1f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex3.png"), 0.12f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex9.png"), 0.24f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex5.png"), 0.14f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex9.png"), 0.2f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex3.png"), 0.14f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex5.png"), 0.6f },
            { AssetManager::Instance().GetAssetPath("LensFlare/tex8.png"), 1.2f }
        };

        auto allocator = AllocatorProvider::Instance().GetAllocator();

        std::vector<std::shared_ptr<LensFlare>> flares{};
        for (const auto& flareCreateInfo : flareCreateInfos) {
            flares.emplace_back(CreateFlare(*allocator, flareCreateInfo.path, flareCreateInfo.scale));
        }

        return std::make_unique<LensFlareComponent>(flares, spacing);
    }

private:
    struct FlareCreateInfo {
        std::string path;
        float scale;
    };

    std::unique_ptr<LensFlare> CreateFlare(Allocator& allocator, const std::string& filePath, const float scale) const
    {
        ImageFactory imageFactory{};
        auto image = imageFactory.CreateImage(filePath);
        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ { image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)image->GetBuffer() });
        return std::make_unique<LensFlare>(std::move(image), std::move(imageBuffer), scale);
    }
};

#endif