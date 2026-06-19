#ifndef __SANDBOX_CAMERA_H__
#define __SANDBOX_CAMERA_H__

#include "../../Tags.h"
#include "../../component/CameraComponent.h"

#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/scene/graph/SceneNode.h>

#include <algorithm>
#include <cstdint>
#include <memory>

namespace sandbox::scene::camera {
class Camera : public prev::scene::graph::SceneNode {
public:
    explicit Camera(uint32_t viewCount)
        : SceneNode({ sandbox::TAG_MAIN_CAMERA })
        , m_viewCount{ std::min(viewCount, static_cast<uint32_t>(MAX_VIEW_COUNT_VALUE)) }
    {
    }

    ~Camera() override = default;

public:
    void Init() override
    {
        m_camera = std::make_shared<sandbox::component::CameraComponent>(m_viewCount);
        prev::scene::component::NodeComponentHelper::AddComponent<sandbox::component::CameraComponent>(GetThis(), m_camera);

        SceneNode::Init();
    }

protected:
    static constexpr float NEAR_CLIPPING_PLANE{ 0.1f };
    static constexpr float FAR_CLIPPING_PLANE{ 100.0f };

    uint32_t m_viewCount{ 1 };
    std::shared_ptr<sandbox::component::CameraComponent> m_camera;
};
} // namespace sandbox::scene::camera

#endif // !__SANDBOX_CAMERA_H__
