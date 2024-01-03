#include "TerrainManagerComponentFactory.h"
#include "TerrainManagerComponent.h"

namespace prev_test::component::terrain {
std::unique_ptr<ITerrainManagerComponent> TerrainManagerComponentFactory::Create() const
{
    return std::make_unique<TerrainManagerComponent>();
}
} // namespace prev_test::component::terrain