#include "WaterComponent.h"
#include "WaterCommon.h"

namespace prev_test::component::water {
WaterComponent::WaterComponent(const int gridX, const int gridZ, const std::shared_ptr<prev_test::render::IMaterial>& material, const std::shared_ptr<prev_test::render::IModel>& model)
    : m_gridX(gridX)
    , m_gridZ(gridZ)
    , m_position(glm::vec3(gridX * 2 * WATER_TILE_SIZE + WATER_TILE_SIZE, WATER_LEVEL, gridZ * 2 * WATER_TILE_SIZE + WATER_TILE_SIZE))
    , m_material(material)
    , m_model(model)
    , m_moveFactor(0.0f)
{
}

std::shared_ptr<prev_test::render::IMaterial> WaterComponent::GetMaterial() const
{
    return m_material;
}

std::shared_ptr<prev_test::render::IModel> WaterComponent::GetModel() const
{
    return m_model;
}

void WaterComponent::Update(float deltaTime)
{
    m_moveFactor += WATER_WAVE_SPEED * deltaTime;
    m_moveFactor = fmodf(m_moveFactor, 1.0f);
}

float WaterComponent::GetMoveFactor() const
{
    return m_moveFactor;
}

const glm::vec3& WaterComponent::GetPosition() const
{
    return m_position;
}

int WaterComponent::GetGridX() const
{
    return m_gridX;
}

int WaterComponent::GetGridZ() const
{
    return m_gridZ;
}
} // namespace prev_test::component::water