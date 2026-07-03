#ifndef __ADAPTERS_H__
#define __ADAPTERS_H__

#include "Adapter.h"

#include <memory>
#include <optional>
#include <vector>

namespace prev::core::device {
class Adapters {
public:
    Adapters(GfxInstance instance);

public:
    // Returns an adapter at hintIndex, or the first discrete/high-performance one if hintIndex < 0.
    // If surface is provided, only adapters that can present to it are considered.
    std::optional<Adapter> Find(GfxSurface surface, int32_t hintIndex = -1) const;

    void Print() const;

    size_t GetCount() const;

    const Adapter& operator[](size_t i) const;

private:
    std::vector<std::unique_ptr<Adapter>> m_adapters;
};
} // namespace prev::core::device

#endif
