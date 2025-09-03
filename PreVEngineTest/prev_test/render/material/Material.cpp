#include "Material.h"

#include <stdexcept>

namespace prev_test::render::material {
Material::Material(const MaterialProperties& materialProps)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
{
}

Material::Material(const MaterialProperties& materialProps, const std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>& images)
    : m_color(materialProps.color)
    , m_shineDamper(materialProps.shineDamper)
    , m_reflectivity(materialProps.reflectivity)
    , m_images(images)
{
}

std::shared_ptr<prev::render::buffer::ImageBuffer> Material::GetImageBuffer(uint32_t index) const
{
    if (index >= m_images.size()) {
        throw std::runtime_error("Invalid image buffer index: " + std::to_string(index));
    }
    return m_images[index];
}

bool Material::HasImageBuffer(uint32_t index)
{
    return index < static_cast<uint32_t>(m_images.size());
}

float Material::GetShineDamper() const
{
    return m_shineDamper;
}

float Material::GetReflectivity() const
{
    return m_reflectivity;
}

uint32_t Material::GetAtlasNumberOfRows() const
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