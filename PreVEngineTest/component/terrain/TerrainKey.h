#ifndef __TERRAIN_KEY__
#define __TERRAIN_KEY__

#include "TerrainCommon.h"

#include <prev/common/Common.h>

namespace prev_test::component::terrain {
struct TerrainKey {
    const int xIndex;

    const int zIndex;

    explicit TerrainKey(const int x, const int z);

    explicit TerrainKey(const glm::vec3& position);

    ~TerrainKey() = default;

    friend bool operator<(const TerrainKey& a, const TerrainKey& b);
};
} // namespace prev_test::component::terrain

#endif // !__TERRAIN_KEY__
