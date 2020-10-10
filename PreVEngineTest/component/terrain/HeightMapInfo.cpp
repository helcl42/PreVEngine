#include "HeightMapInfo.h"

namespace prev_test::component::terrain {
HeightMapInfo::HeightMapInfo(const size_t size)
{
    heights.resize(size);
    for (size_t i = 0; i < size; i++) {
        heights[i] = std::vector<float>(size);
    }
}

float HeightMapInfo::GetHeightAt(const int32_t x, const int32_t z) const
{
    const int32_t coordX = glm::clamp(x, 0, static_cast<int32_t>(heights.size() - 1));
    const int32_t coordZ = glm::clamp(z, 0, static_cast<int32_t>(heights.size() - 1));

    return heights[coordX][coordZ];
}

size_t HeightMapInfo::GetSize() const
{
    return heights.size();
}

float HeightMapInfo::GetMinHeight() const
{
    return minHeight;
}

float HeightMapInfo::GetMaxHeight() const
{
    return maxHeight;
}

float HeightMapInfo::GetGlobalMinHeight() const
{
    return globalMinHeight;
}

float HeightMapInfo::GetGlobalMaxHeight() const
{
    return globalMaxHeight;
}
} // namespace prev_test::component::terrain