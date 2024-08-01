#ifndef __PLANE_NODE_H__
#define __PLANE_NODE_H__

#include "../General.h"
#include "../component/ray_casting/IBoundingVolumeComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Plane final : public prev::scene::graph::SceneNode {
public:
    Plane(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const float heightScale);

    ~Plane() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    glm::vec3 m_initialPosition;

    glm::quat m_initialOrientation;

    glm::vec3 m_initialScale;

    std::string m_texturePath;

    std::string m_normalMapPath;

    std::string m_heightMapPath;

    float m_heightScale;

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__PLANE_NODE_H__
