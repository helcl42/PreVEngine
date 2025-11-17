#include "TerrainKey.h"

namespace prev_test::component::terrain {
TerrainKey::TerrainKey(const int x, const int z)
    : xIndex(x)
    , zIndex(z)
{
}

TerrainKey::TerrainKey(const glm::vec3& position)
    : xIndex(static_cast<int>(position.x / TERRAIN_TILE_SIZE))
    , zIndex(static_cast<int>(position.z / TERRAIN_TILE_SIZE))
{
}

bool operator<(const TerrainKey& a, const TerrainKey& b)
{
    if (a.xIndex < b.xIndex) {
        return true;
    }
    if (a.xIndex > b.xIndex) {
        return false;
    }

    if (a.zIndex < b.zIndex) {
        return true;
    }
    if (a.zIndex > b.zIndex) {
        return false;
    }

    return false;
}
} // namespace prev_test::component::terrain