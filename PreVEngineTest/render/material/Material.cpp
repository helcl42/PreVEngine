#include "Material.h"

namespace prev_test::render::material {
Material::Material(const MaterialProperties& materialProps)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
{
}

Material::Material(const MaterialProperties& materialProps, const ImagePair& image)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
    , m_image(image.image)
    , m_imageBuffer(image.imageBuffer)
{
}

Material::Material(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
    , m_image(image.image)
    , m_imageBuffer(image.imageBuffer)
    , m_normalImage(normalMap.image)
    , m_normalImageBuffer(normalMap.imageBuffer)
{
}

Material::Material(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap, const ImagePair& heightMap)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
    , m_image(image.image)
    , m_imageBuffer(image.imageBuffer)
    , m_normalImage(normalMap.image)
    , m_normalImageBuffer(normalMap.imageBuffer)
    , m_heightmage(heightMap.image)
    , m_heightImageBuffer(heightMap.imageBuffer)
{
}

std::shared_ptr<prev::render::image::Image> Material::GetImage() const
{
    return m_image;
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> Material::GetImageBuffer() const
{
    return m_imageBuffer;
}

bool Material::HasImage() const
{
    return m_image != nullptr;
}

const glm::vec4& Material::GetColor() const
{
    return m_color;
}

std::shared_ptr<prev::render::image::Image> Material::GetNormalImage() const
{
    return m_normalImage;
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> Material::GetNormalmageBuffer() const
{
    return m_normalImageBuffer;
}

bool Material::HasNormalImage() const
{
    return m_normalImage != nullptr;
}

std::shared_ptr<prev::render::image::Image> Material::GetHeightImage() const
{
    return m_heightmage;
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> Material::GetHeightImageBuffer() const
{
    return m_heightImageBuffer;
}

bool Material::HasHeightImage() const
{
    return m_heightmage != nullptr;
}

float Material::GetShineDamper() const
{
    return m_shineDamper;
}

float Material::GetReflectivity() const
{
    return m_reflectivity;
}

bool Material::HasTransparency() const
{
    return m_hasTransparency;
}

void Material::SetHasTransparency(const bool transparency)
{
    m_hasTransparency = transparency;
}

bool Material::UsesFakeLightning() const
{
    return m_usesFakeLightning;
}

void Material::SetUsesFakeLightning(const bool fake)
{
    m_usesFakeLightning = fake;
}

unsigned int Material::GetAtlasNumberOfRows() const
{
    return m_atlasNumberOfRows;
}

void Material::SetAtlasNumberOfRows(const uint32_t rows)
{
    m_atlasNumberOfRows = rows;
}

const glm::vec2& Material::GetTextureOffset() const
{
    return m_textureOffset;
}

void Material::SetTextureOffset(const glm::vec2& textureOffset)
{
    m_textureOffset = textureOffset;
}

float Material::GetHeightScale() const
{
    return m_heightScale;
}

void Material::SetHeightScale(const float scale)
{
    m_heightScale = scale;
}
} // namespace prev_test::render::material