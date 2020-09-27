#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "../IMaterial.h"

namespace prev_test::render::material {
class Material : public IMaterial {
public:
    Material(const MaterialProperties& materialPros);

    Material(const MaterialProperties& materialProps, const ImagePair& image);

    Material(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap);

    Material(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap, const ImagePair& heightMap);

    virtual ~Material() = default;

public:
    std::shared_ptr<prev::render::image::Image> GetImage() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetImageBuffer() const override;

    bool HasImage() const override;

    const glm::vec3& GetColor() const override;

    std::shared_ptr<prev::render::image::Image> GetNormalImage() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetNormalmageBuffer() const override;

    bool HasNormalImage() const override;

    std::shared_ptr<prev::render::image::Image> GetHeightImage() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetHeightImageBuffer() const override;

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

    float GetHeightScale() const override;

    void SetHeightScale(const float scale) override;

private:
    glm::vec3 m_color{ 1.0f, 1.0f, 1.0f };

    float m_shineDamper{ 10.0f };

    float m_reflectivity{ 1.0f };

    std::shared_ptr<prev::render::image::Image> m_image{ nullptr };

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_imageBuffer{ nullptr };

    std::shared_ptr<prev::render::image::Image> m_normalImage{ nullptr };

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_normalImageBuffer{ nullptr };

    std::shared_ptr<prev::render::image::Image> m_heightmage{ nullptr };

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_heightImageBuffer{ nullptr };

    bool m_hasTransparency{ false };

    bool m_usesFakeLightning{ false };

    uint32_t m_atlasNumberOfRows{ 1 };

    glm::vec2 m_textureOffset{ 0.0f, 0.0f };

    float m_heightScale{ 0.1f };
};
} // namespace prev_test::render::material

#endif // !__MATERIAL_H__
