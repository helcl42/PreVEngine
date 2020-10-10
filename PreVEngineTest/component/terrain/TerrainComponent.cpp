#include "TerrainComponent.h"

#include <prev/util/MathUtils.h>

namespace prev_test::component::terrain {
TerrainComponent::TerrainComponent(const int gridX, const int gridZ)
    : m_gridX(gridX)
    , m_gridZ(gridZ)
    , m_position(glm::vec3(gridX * TERRAIN_TILE_SIZE, 0.0f, gridZ * TERRAIN_TILE_SIZE))
{
}

std::shared_ptr<prev_test::render::IModel> TerrainComponent::GetModel() const
{
    return m_model;
}

std::vector<std::shared_ptr<prev_test::render::IMaterial> > TerrainComponent::GetMaterials() const
{
    return m_materials;
}

bool TerrainComponent::GetHeightAt(const glm::vec3& position, float& outHeight) const
{
    const float terrainX = position.x - m_position.x;
    const float terrainZ = position.z - m_position.z;
    const float gridSquareSize = TERRAIN_TILE_SIZE / (static_cast<float>(m_heightsInfo->GetSize()) - 1.0f);
    const int gridX = static_cast<int>(floorf(terrainX / gridSquareSize));
    const int gridZ = static_cast<int>(floorf(terrainZ / gridSquareSize));

    if (gridX >= static_cast<int>(m_heightsInfo->GetSize()) - 1 || gridX < 0 || gridZ >= static_cast<int>(m_heightsInfo->GetSize()) - 1 || gridZ < 0) {
        return false;
    }

    const float xCoord = fmodf(terrainX, gridSquareSize) / gridSquareSize;
    const float zCoord = fmodf(terrainZ, gridSquareSize) / gridSquareSize;

    if (xCoord <= (1 - zCoord)) {
        outHeight = prev::util::MathUtil::BarryCentric(glm::vec3(0, m_heightsInfo->GetHeightAt(gridX, gridZ), 0), glm::vec3(1, m_heightsInfo->GetHeightAt(gridX + 1, gridZ), 0), glm::vec3(0, m_heightsInfo->GetHeightAt(gridX, gridZ + 1), 1), glm::vec2(xCoord, zCoord));
    } else {
        outHeight = prev::util::MathUtil::BarryCentric(glm::vec3(1, m_heightsInfo->GetHeightAt(gridX + 1, gridZ), 0), glm::vec3(1, m_heightsInfo->GetHeightAt(gridX + 1, gridZ + 1), 1), glm::vec3(0, m_heightsInfo->GetHeightAt(gridX, gridZ + 1), 1), glm::vec2(xCoord, zCoord));
    }
    return true;
}

std::shared_ptr<VertexData> TerrainComponent::GetVertexData() const
{
    return m_vertexData;
}

const glm::vec3& TerrainComponent::GetPosition() const
{
    return m_position;
}

std::shared_ptr<HeightMapInfo> TerrainComponent::GetHeightMapInfo() const
{
    return m_heightsInfo;
}

int TerrainComponent::GetGridX() const
{
    return m_gridX;
}

int TerrainComponent::GetGridZ() const
{
    return m_gridZ;
}

std::vector<float> TerrainComponent::GetHeightSteps() const
{
    return m_heightSteps;
}

float TerrainComponent::GetTransitionRange() const
{
    return m_transitionRange;
}
} // namespace prev_test::component::terrain