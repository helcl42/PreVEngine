#ifndef __WATER_MANAGER_H__
#define __WATER_MANAGER_H__

#include "../../General.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::water {
class WaterManager final : public prev::scene::graph::SceneNode {
public:
    WaterManager(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const int maxX, const int maxZ, const uint32_t viewCount);

    ~WaterManager() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    int m_gridMaxX;

    int m_gridMaxZ;

    uint32_t m_viewCount;
};
} // namespace prev_test::scene::water

#endif // !__WATER_MANAGER_H__