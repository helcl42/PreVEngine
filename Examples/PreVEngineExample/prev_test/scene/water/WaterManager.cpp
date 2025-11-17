#include "WaterManager.h"
#include "Water.h"
#include "WaterReflection.h"
#include "WaterRefraction.h"

namespace prev_test::scene::water {
WaterManager::WaterManager(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const int maxX, const int maxZ, const uint32_t viewCount)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
    , m_gridMaxX(maxX)
    , m_gridMaxZ(maxZ)
    , m_viewCount(viewCount)
{
}

void WaterManager::Init()
{
    for (int x = 0; x < m_gridMaxX; x++) {
        for (int z = 0; z < m_gridMaxZ; z++) {
            auto waterTile = std::make_shared<Water>(m_device, m_allocator, x, z);
            AddChild(waterTile);
        }
    }

    auto waterReflection = std::make_shared<WaterReflection>(m_device, m_allocator, m_viewCount);
    AddChild(waterReflection);

    auto waterRefraction = std::make_shared<WaterRefraction>(m_device, m_allocator, m_viewCount);
    AddChild(waterRefraction);

    SceneNode::Init();
}

void WaterManager::Update(float deltaTime)
{
    SceneNode::Update(deltaTime);
}

void WaterManager::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::water
