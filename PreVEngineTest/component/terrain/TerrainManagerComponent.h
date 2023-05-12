#ifndef __TERRAIN_MANAGER_COMPONENT_H__
#define __TERRAIN_MANAGER_COMPONENT_H__

#include "ITerrainManagerComponent.h"
#include "TerrainKey.h"

#include <map>

namespace prev_test::component::terrain {
class TerrainManagerComponent : public ITerrainManagerComponent {
public:
    void AddTerrainComponent(const std::shared_ptr<ITerrainComponenet>& terrain) override;

    void RemoveTerrain(const std::shared_ptr<ITerrainComponenet>& terrain) override;

    std::shared_ptr<ITerrainComponenet> GetTerrainAt(const glm::vec3& position) const override;

    bool GetHeightAt(const glm::vec3& position, float& outHeight) const override;

private:
    std::map<TerrainKey, std::weak_ptr<ITerrainComponenet>> m_terrains;
};
} // namespace prev_test::component::terrain

#endif // !__TERRAIN_MANAGER_COMPONENT_H__
