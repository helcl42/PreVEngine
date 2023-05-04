#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "../IMaterial.h"

namespace prev_test::render::material {
struct ImagePair {
    std::shared_ptr<prev::core::memory::image::IImageBuffer> imageBuffer;
    std::shared_ptr<prev::render::sampler::Sampler> imageSampler;
};

class Material : public IMaterial {
public:
    Material(const MaterialProperties& materialPros);

    Material(const MaterialProperties& materialProps, const ImagePair& image);

    Material(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap);

    Material(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap, const ImagePair& heightMap);

    virtual ~Material() = default;

public:
    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetImageBuffer() const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetImageSampler() const override;

    bool HasImage() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetNormalmageBuffer() const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetNormalImageSampler() const override;

    bool HasNormalImage() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetHeightImageBuffer() const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetHeightImageSampler() const override;

    bool HasHeightImage() const override;

    float GetShineDamper() const override;

    float GetReflectivity() const override;

    bool HasTransparency() const override;

    void SetHasTransparency(const bool transparency) override;

    bool UsesFakeLightning() const override;

    void SetUsesFakeLightning(const bool fake) override;

    unsigned int GetAtlasNumberOfRows() const override;

    void SetAtlasNumberOfRows(const uint32_t rows) override;

    const glm::vec2& GetTextureOffset() const override;

    void SetTextureOffset(const glm::vec2& textureOffset) override;

    const glm::vec4& GetColor() const override;

    float GetHeightScale() const override;

    void SetHeightScale(const float scale) override;

private:
    glm::vec4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };

    float m_shineDamper{ 10.0f };

    float m_reflectivity{ 1.0f };

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_imageBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_imageSampler{};

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_normalImageBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_normalImageSampler{};

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_heightImageBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_heightImageSampler{};

    bool m_hasTransparency{ false };

    bool m_usesFakeLightning{ false };

    uint32_t m_atlasNumberOfRows{ 1 };

    glm::vec2 m_textureOffset{ 0.0f, 0.0f };

    float m_heightScale{ 0.1f };
};
} // namespace prev_test::render::material

#endif // !__MATERIAL_H__
