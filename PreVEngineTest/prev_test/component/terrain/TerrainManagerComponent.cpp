#include "TerrainManagerComponent.h"

namespace prev_test::component::terrain {
void TerrainManagerComponent::AddTerrainComponent(const std::shared_ptr<ITerrainComponent>& terrain)
{
    m_terrains.insert({ TerrainKey{ terrain->GetPosition() }, terrain });
}

void TerrainManagerComponent::RemoveTerrain(const std::shared_ptr<ITerrainComponent>& terrain)
{
    m_terrains.erase(TerrainKey{ terrain->GetPosition() });
}

std::shared_ptr<ITerrainComponent> TerrainManagerComponent::GetTerrainAt(const glm::vec3& position) const
{
    const TerrainKey key{ position };

    auto terrainIter = m_terrains.find(key);
    if (terrainIter != m_terrains.cend()) {
        return terrainIter->second.lock();
    }
    return nullptr;
}

bool TerrainManagerComponent::GetHeightAt(const glm::vec3& position, float& outHeight) const
{
    const auto terrain = GetTerrainAt(position);
    if (terrain) {
        return terrain->GetHeightAt(position, outHeight);
    }
    return false;
}
} // namespace prev_test::component::terrain