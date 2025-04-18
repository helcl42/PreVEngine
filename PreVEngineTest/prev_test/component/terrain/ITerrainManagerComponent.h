#ifndef __ITERRAIN_MANAGER_COMPONENT_H__
#define __ITERRAIN_MANAGER_COMPONENT_H__

#include "ITerrainComponent.h"

#include <prev/scene/component/IComponent.h>

namespace prev_test::component::terrain {
class ITerrainManagerComponent : public prev::scene::component::IComponent {
public:
    virtual void AddTerrainComponent(const std::shared_ptr<ITerrainComponent>& terrain) = 0;

    virtual void RemoveTerrain(const std::shared_ptr<ITerrainComponent>& terrain) = 0;

    virtual std::shared_ptr<ITerrainComponent> GetTerrainAt(const glm::vec3& position) const = 0;

    virtual bool GetHeightAt(const glm::vec3& position, float& outHeight) const = 0;

public:
    virtual ~ITerrainManagerComponent() = default;
};
} // namespace prev_test::component::terrain

#endif // !__ITERRAIN_MANAGER_COMPONENT_H__
