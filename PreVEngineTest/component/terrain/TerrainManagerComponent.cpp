#include "TerrainManagerComponent.h"

namespace prev_test::component::terrain {
void TerrainManagerComponent::AddTerrainComponent(const std::shared_ptr<ITerrainComponenet>& terrain)
{
    m_terrains.insert({ TerrainKey{ terrain->GetPosition() }, terrain });
}

void TerrainManagerComponent::RemoveTerrain(const std::shared_ptr<ITerrainComponenet>& terrain)
{
    m_terrains.erase(TerrainKey{ terrain->GetPosition() });
}

std::shared_ptr<ITerrainComponenet> TerrainManagerComponent::GetTerrainAt(const glm::vec3& position) const
{
    const TerrainKey key{ position };
    if (m_terrains.find(key) != m_terrains.cend()) {
        return m_terrains.at(key).lock();
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