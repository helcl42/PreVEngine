#include "SphereBoundingVolumeComponent.h"

#include "../../common/intersection/IntersectionTester.h"

#ifdef RENDER_BOUNDING_VOLUMES
#include "BoundingVolumeModelFactory.h"
#endif

#include <prev/util/MathUtils.h>

namespace prev_test::component::ray_casting {
SphereBoundingVolumeComponent::SphereBoundingVolumeComponent(prev::core::memory::Allocator& allocator, const prev_test::common::intersection::Sphere& sphere, const float scale, const glm::vec3& offset)
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

bool SphereBoundingVolumeComponent::IsInFrustum(const prev_test::common::intersection::Frustum& frustum)
{
    return prev_test::common::intersection::IntersectionTester::Intersects(frustum, m_working);
}

bool SphereBoundingVolumeComponent::Intersects(const prev_test::common::intersection::Ray& ray, prev_test::common::intersection::RayCastResult& result)
{
    return prev_test::common::intersection::IntersectionTester::Intersects(ray, m_working, result);
}

void SphereBoundingVolumeComponent::Update(const glm::mat4& worldTransform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    prev::util::math::DecomposeTransform(worldTransform, rotation, translation, scale);

    m_working = prev_test::common::intersection::Sphere{ translation + m_original.position * scale, m_original.radius * glm::length(scale) };
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

prev_test::common::intersection::Sphere SphereBoundingVolumeComponent::ScaleSphere(const prev_test::common::intersection::Sphere& sphere, const float scale)
{
    return prev_test::common::intersection::Sphere{ sphere.position, sphere.radius * scale };
}

prev_test::common::intersection::Sphere SphereBoundingVolumeComponent::OffsetSphere(const prev_test::common::intersection::Sphere& sphere, const glm::vec3& offset)
{
    return prev_test::common::intersection::Sphere{ sphere.position + offset, sphere.radius };
}
} // namespace prev_test::component::ray_casting