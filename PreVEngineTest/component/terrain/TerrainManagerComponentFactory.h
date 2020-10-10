#ifndef __TERRAIN_MANAGER_COMPONENT_FACTORY_H__
#define __TERRAIN_MANAGER_COMPONENT_FACTORY_H__

#include "ITerrainManagerComponent.h"

namespace prev_test::component::terrain {
class TerrainManagerComponentFactory final {
public:
    std::unique_ptr<ITerrainManagerComponent> TerrainManagerComponentFactory::Create() const;
};
} // namespace prev_test::component::terrain

#endif // !__TERRAIN_MANAGER_COMPONENT_FACTORY_H__
