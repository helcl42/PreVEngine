#ifndef __TERRAIN_MANAGER_H__
#define __TERRAIN_MANAGER_H__

#include "../../component/terrain/TerrainCommon.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::terrain {
class TerrainManager final : public prev::scene::graph::SceneNode {
public:
    TerrainManager(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const uint32_t maxX, const uint32_t maxZ);

    virtual ~TerrainManager() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    uint32_t m_gridMaxX;

    uint32_t m_gridMaxZ;
};
} // namespace prev_test::scene::terrain

#endif // !__TERRAIN_MANAGER_H__
