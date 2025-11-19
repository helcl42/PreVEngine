#include "SphereBoundingVolumeComponent.h"

#ifdef RENDER_BOUNDING_VOLUMES
#include "BoundingVolumeModelFactory.h"
#endif

#include <prev/util/MathUtils.h>
#include <prev/util/intersection/IntersectionTester.h>

namespace prev_test::component::ray_casting {
SphereBoundingVolumeComponent::SphereBoundingVolumeComponent(prev::core::memory::Allocator& allocator, const prev::util::intersection::Sphere& sphere, const float scale, const glm::vec3& offset)
    : m_allocator{ allocator }
    , m_scale{ scale }
    , m_offset{ offset }
{
    auto newSphere = OffsetSphere(sphere, offset);
    newSphere = ScaleSphere(newSphere, scale);
    m_original = newSphere;
    m_working = newSphere;

#ifdef RENDER_BOUNDING_VOLUMES
    m_model = BoundingVolumeModelFactory{ m_allocator }.CreateSphereModel(m_working);
#endif
}

bool SphereBoundingVolumeComponent::IsInFrustum(const prev::util::intersection::Frustum& frustum)
{
    return prev::util::intersection::tester::Intersects(frustum, m_working);
}

bool SphereBoundingVolumeComponent::Intersects(const prev::util::intersection::Ray& ray, prev::util::intersection::RayCastResult& result)
{
    return prev::util::intersection::tester::Intersects(ray, m_working, result);
}

void SphereBoundingVolumeComponent::Update(const glm::mat4& worldTransform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    prev::util::math::DecomposeTransform(worldTransform, rotation, translation, scale);

    m_working = prev::util::intersection::Sphere{ translation + m_original.position * scale, m_original.radius * glm::length(scale) };
#ifdef RENDER_BOUNDING_VOLUMES
    m_model = BoundingVolumeModelFactory{ m_allocator }.CreateSphereModel(m_working, m_model);
#endif
}

BoundingVolumeType SphereBoundingVolumeComponent::GetType() const
{
    return BoundingVolumeType::SPHERE;
}

#ifdef RENDER_BOUNDING_VOLUMES
std::shared_ptr<prev_test::render::IModel> SphereBoundingVolumeComponent::GetModel() const
{
    return m_model;
}
#endif

prev::util::intersection::Sphere SphereBoundingVolumeComponent::ScaleSphere(const prev::util::intersection::Sphere& sphere, const float scale)
{
    return prev::util::intersection::Sphere{ sphere.position, sphere.radius * scale };
}

prev::util::intersection::Sphere SphereBoundingVolumeComponent::OffsetSphere(const prev::util::intersection::Sphere& sphere, const glm::vec3& offset)
{
    return prev::util::intersection::Sphere{ sphere.position + offset, sphere.radius };
}
} // namespace prev_test::component::ray_casting