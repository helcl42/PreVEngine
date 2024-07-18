#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "../IMaterial.h"

#include <vector>

namespace prev_test::render::material {
struct ImagePair {
    std::shared_ptr<prev::render::buffer::ImageBuffer> imageBuffer{};
    std::shared_ptr<prev::render::sampler::Sampler> imageSampler{};
};

class Material : public IMaterial {
public:
    Material(const MaterialProperties& materialPros);

    Material(const MaterialProperties& materialProps, const std::vector<ImagePair>& imagePairs);

    virtual ~Material() = default;

public:
    std::shared_ptr<prev::render::buffer::ImageBuffer> GetImageBuffer(uint32_t index = 0) const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetSampler(uint32_t index = 0) const override;

    bool HasImageBuffer(uint32_t index) override;

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

    std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>> m_imageBuffers;

    std::vector<std::shared_ptr<prev::render::sampler::Sampler>> m_samplers;

    bool m_hasTransparency{ false };

    bool m_usesFakeLightning{ false };

    uint32_t m_atlasNumberOfRows{ 1 };

    glm::vec2 m_textureOffset{ 0.0f, 0.0f };

    float m_heightScale{ 0.1f };
};
} // namespace prev_test::render::material

#endif // !__MATERIAL_H__
