#ifndef __CUBE_H__
#define __CUBE_H__

#include "../component/ray_casting/IBoundingVolumeComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Cube final : public prev::scene::graph::SceneNode {
public:
    Cube(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const float heightScale);

    ~Cube() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

protected:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    glm::vec3 m_initialPosition;

    glm::quat m_initialOrientation;

    glm::vec3 m_initialScale;

    std::string m_texturePath;

    std::string m_normalMapPath;

    std::string m_heightMapPath;

    float m_heightScale;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__CUBE_H__
