#ifndef __HEIGHT_MAP_INFO_H__
#define __HEIGHT_MAP_INFO_H__

#include <prev/common/Common.h>

namespace prev_test::component::terrain {
struct HeightMapInfo {
    std::vector<std::vector<float> > heights;

    float minHeight{ 0.0f };

    float maxHeight{ 0.0f };

    float globalMinHeight{ 0.0f };

    float globalMaxHeight{ 0.0f };

    HeightMapInfo() = default;

    HeightMapInfo(const size_t size);

    float GetHeightAt(const int32_t x, const int32_t z) const;

    size_t GetSize() const;

    float GetMinHeight() const;

    float GetMaxHeight() const;

    float GetGlobalMinHeight() const;

    float GetGlobalMaxHeight() const;
};
} // namespace prev_test::component::terrain

#endif // !__HEIGHT_MAP_INFO_H__
