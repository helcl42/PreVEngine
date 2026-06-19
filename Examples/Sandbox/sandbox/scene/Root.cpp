#include "Root.h"

#include "../render/ModelFactory.h"
#include "Object.h"
#include "camera/CameraFactory.h"

#include <glm/glm.hpp>

#include <memory>

namespace sandbox::scene {
Root::Root(prev::core::device::Device& device, uint32_t viewCount)
    : SceneNode()
    , m_device{ device }
    , m_viewCount{ viewCount }
{
}

void Root::Init()
{
    AddChild(camera::CameraFactory::Create(m_viewCount));

    // Build the sandbox geometry once and share it across every sandbox node (unique -> shared).
    const std::shared_ptr<sandbox::render::Model> sandboxModel{ sandbox::render::ModelFactory::CreateSandbox(m_device) };

    // A 3x3x3 grid of cubes centered on the origin, each with a distinct color (axis -> channel).
    constexpr int gridSize{ 3 };
    constexpr float spacing{ 2.5f };
    const float offset{ (gridSize - 1) * spacing * 0.5f };
    const float denom{ static_cast<float>(gridSize - 1) };
    for (int x = 0; x < gridSize; ++x) {
        for (int y = 0; y < gridSize; ++y) {
            for (int z = 0; z < gridSize; ++z) {
                const glm::vec3 position{ x * spacing - offset, y * spacing - offset, z * spacing - offset };
                const glm::vec4 color{
                    0.2f + 0.8f * (static_cast<float>(x) / denom),
                    0.2f + 0.8f * (static_cast<float>(y) / denom),
                    0.2f + 0.8f * (static_cast<float>(z) / denom),
                    1.0f
                };
                AddChild(std::make_shared<Object>(sandboxModel, position, color));
            }
        }
    }

    SceneNode::Init();
}
} // namespace sandbox::scene
