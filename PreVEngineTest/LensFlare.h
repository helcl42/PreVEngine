#ifndef __LENS_FLARE_H__
#define __LENS_FLARE_H__

#include "General.h"

#include "render/IMesh.h"
#include "common/AssetManager.h"

#include <prev/render/image/ImageFactory.h>

class Flare {
public:
    Flare(const std::shared_ptr<prev::render::image::Image>& image, const std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer, const float scale)
        : m_image(image)
        , m_imageBuffer(imageBuffer)
        , m_scale(scale)
        , m_screenSpacePosition({ 0.0f, 0.0f })
    {
    }

    ~Flare() = default;

public:
    std::shared_ptr<prev::render::image::Image> GetImage() const
    {
        return m_image;
    }

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetImageBuffer() const
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
    std::shared_ptr<prev::render::image::Image> m_image;

    std::shared_ptr<prev::core::memory::image::ImageBuffer> m_imageBuffer;

    float m_scale;

    glm::vec2 m_screenSpacePosition;
};

class QuadMesh2D : public prev_test::render::IMesh {
public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override
    {
        return vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return (const void*)vertices.data();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return vertices3d;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return indices;
    }

    const std::vector<prev_test::render::MeshPart>& GetMeshParts() const override
    {
        return meshParts;
    }

private:
    static const inline prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC2 } };

    static const inline std::vector<glm::vec2> vertices = {
        { 0.5f, 0.5f },
        { -0.5f, 0.5f },
        { -0.5f, -0.5f },
        { 0.5f, -0.5f }
    };

    static const inline std::vector<glm::vec3> vertices3d = {
        { vertices[0], 0.0f },
        { vertices[1], 0.0f },
        { vertices[2], 0.0f },
        { vertices[3], 0.0f }
    };

    static const inline std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    static const inline std::vector<prev_test::render::MeshPart> meshParts = {
        prev_test::render::MeshPart(static_cast<uint32_t>(indices.size()))
    };
};

class ILensFlareComponent {
public:
    virtual void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) = 0;

    virtual const std::vector<std::shared_ptr<Flare> >& GetFlares() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

public:
    virtual ~ILensFlareComponent() = default;
};

class LensFlareComponent : public ILensFlareComponent {
public:
    explicit LensFlareComponent(const std::vector<std::shared_ptr<Flare> >& flares, float spacing, const std::shared_ptr<prev_test::render::IModel>& model)
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

    std::shared_ptr<prev_test::render::IModel> GetModel() const override
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

    std::shared_ptr<prev_test::render::IModel> m_model;
};

class LensFlareComponentFactory {
public:
    std::unique_ptr<ILensFlareComponent> Create() const
    {
        const float spacing{ 0.16f };
        const std::vector<FlareCreateInfo> flareCreateInfos = {
            { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex2.png"), 0.0f },
            { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex3.png"), 0.12f },
            { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex4.png"), 0.46f },
            { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex5.png"), 0.12f },
            { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex6.png"), 0.0f },
            { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex7.png"), 0.1f },
            { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex8.png"), 1.2f },
            { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex9.png"), 0.24f },
        };

        auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

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

    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const
    {
        auto mesh = std::make_unique<QuadMesh2D>();
        auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
        vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));
        return std::make_unique<prev_test::render::model::Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    std::unique_ptr<Flare> CreateFlare(prev::core::memory::Allocator& allocator, const std::string& filePath, const float scale) const
    {
        prev::render::image::ImageFactory imageFactory{};
        auto image = imageFactory.CreateImage(filePath);
        auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
        imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
        return std::make_unique<Flare>(std::move(image), std::move(imageBuffer), scale);
    }
};

class ISunComponent {
public:
    virtual void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) = 0;

    virtual std::shared_ptr<Flare> GetFlare() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

public:
    virtual ~ISunComponent() = default;
};

class SunComponent : public ISunComponent {
public:
    explicit SunComponent(const std::shared_ptr<Flare>& flare, const std::shared_ptr<prev_test::render::IModel>& model)
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

    std::shared_ptr<prev_test::render::IModel> GetModel() const override
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

    std::shared_ptr<prev_test::render::IModel> m_model;
};

class SunComponentFactory {
public:
    std::unique_ptr<ISunComponent> Create() const
    {
        auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

        auto flare = CreateFlare(*allocator, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sun.png"), 0.2f);
        flare->SetScreenSpacePosition(glm::vec2(-100.0f, -100.0f));

        auto model = CreateModel(*allocator);
        return std::make_unique<SunComponent>(std::move(flare), std::move(model));
    }

private:
    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const
    {
        auto mesh = std::make_unique<QuadMesh2D>();
        auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
        vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

        return std::make_unique<prev_test::render::model::Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    std::unique_ptr<Flare> CreateFlare(prev::core::memory::Allocator& allocator, const std::string& filePath, const float scale) const
    {
        prev::render::image::ImageFactory imageFactory{};
        auto image = imageFactory.CreateImage(filePath);
        auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
        imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
        return std::make_unique<Flare>(std::move(image), std::move(imageBuffer), scale);
    }
};

#endif