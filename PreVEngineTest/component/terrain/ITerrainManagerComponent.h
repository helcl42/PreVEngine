#ifndef __ITERRAIN_MANAGER_COMPONENT_H__
#define __ITERRAIN_MANAGER_COMPONENT_H__

#include "ITerrainComponent.h"

namespace prev_test::component::terrain {
class ITerrainManagerComponent {
public:
    virtual void AddTerrainComponent(const std::shared_ptr<ITerrainComponenet>& terrain) = 0;

    virtual void RemoveTerrain(const std::shared_ptr<ITerrainComponenet>& terrain) = 0;

    virtual std::shared_ptr<ITerrainComponenet> GetTerrainAt(const glm::vec3& position) const = 0;

    virtual bool GetHeightAt(const glm::vec3& position, float& outHeight) const = 0;

public:
    virtual ~ITerrainManagerComponent() = default;
};
} // namespace prev_test::component::terrain

#endif // !__ITERRAIN_MANAGER_COMPONENT_H__
