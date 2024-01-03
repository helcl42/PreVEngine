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
    , m_imageBuffer(image.imageBuffer)
    , m_imageSampler(image.imageSampler)
{
}

Material::Material(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
    , m_imageBuffer(image.imageBuffer)
    , m_imageSampler(image.imageSampler)
    , m_normalImageBuffer(normalMap.imageBuffer)
    , m_normalImageSampler(normalMap.imageSampler)
{
}

Material::Material(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap, const ImagePair& heightMap)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
    , m_imageBuffer(image.imageBuffer)
    , m_imageSampler(image.imageSampler)
    , m_normalImageBuffer(normalMap.imageBuffer)
    , m_normalImageSampler(normalMap.imageSampler)
    , m_heightImageBuffer(heightMap.imageBuffer)
    , m_heightImageSampler(heightMap.imageSampler)
{
}

std::shared_ptr<prev::render::buffer::image::IImageBuffer> Material::GetImageBuffer() const
{
    return m_imageBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> Material::GetImageSampler() const
{
    return m_imageSampler;
}

bool Material::HasImage() const
{
    return m_imageBuffer != nullptr;
}

std::shared_ptr<prev::render::buffer::image::IImageBuffer> Material::GetNormalmageBuffer() const
{
    return m_normalImageBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> Material::GetNormalImageSampler() const
{
    return m_normalImageSampler;
}

bool Material::HasNormalImage() const
{
    return m_normalImageBuffer != nullptr;
}

std::shared_ptr<prev::render::buffer::image::IImageBuffer> Material::GetHeightImageBuffer() const
{
    return m_heightImageBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> Material::GetHeightImageSampler() const
{
    return m_heightImageSampler;
}

bool Material::HasHeightImage() const
{
    return m_heightImageBuffer != nullptr;
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

const glm::vec4& Material::GetColor() const
{
    return m_color;
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