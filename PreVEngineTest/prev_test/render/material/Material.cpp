#include "Material.h"

#include <stdexcept>

namespace prev_test::render::material {
Material::Material(const MaterialProperties& materialProps)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
{
}

Material::Material(const MaterialProperties& materialProps, const std::vector<ImagePair>& imagePairs)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
{
    for (const auto& imagePair : imagePairs) {
        if (imagePair.imageBuffer) {
            m_imageBuffers.emplace_back(imagePair.imageBuffer);
            m_samplers.emplace_back(imagePair.imageSampler);
        }
    }
}

std::shared_ptr<prev::render::buffer::image::IImageBuffer> Material::GetImageBuffer(uint32_t index) const
{
    if (index >= m_imageBuffers.size()) {
        throw std::runtime_error("Invalid image buffer index: " + std::to_string(index));
    }
    return m_imageBuffers[index];
}

std::shared_ptr<prev::render::sampler::Sampler> Material::GetSampler(uint32_t index) const
{
    if (index >= m_samplers.size()) {
        throw std::runtime_error("Invalid sampler index: " + std::to_string(index));
    }
    return m_samplers[index];
}

bool Material::HasImageBuffer(uint32_t index)
{
    return index < static_cast<uint32_t>(m_imageBuffers.size());
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