#include "OBBBoundingVolumeComponent.h"

#include "../../common/intersection/IntersectionTester.h"

#ifdef RENDER_BOUNDING_VOLUMES
#include "BoundingVolumeModelFactory.h"
#endif

#include <prev/util/MathUtils.h>

namespace prev_test::component::ray_casting {
OBBBoundingVolumeComponent::OBBBoundingVolumeComponent(prev::core::memory::Allocator& allocator, const prev_test::common::intersection::OBB& obb, const glm::vec3& scale, const glm::vec3& offset)
    : m_allocator{ allocator }
    , m_scale{ scale }
    , m_offset{ offset }
{
    auto newBox = OffsetOBB(obb, offset);
    newBox = ScaleOBB(newBox, scale);

    m_original = newBox;
    m_working = newBox;

#ifdef RENDER_BOUNDING_VOLUMES
    m_model = BoundingVolumeModelFactory{ m_allocator }.CreateOBBModel(m_working);
#endif
}

bool OBBBoundingVolumeComponent::IsInFrustum(const prev_test::common::intersection::Frustum& frustum)
{
    return prev_test::common::intersection::IntersectionTester::Intersects(m_working, frustum);
}

bool OBBBoundingVolumeComponent::Intersects(const prev_test::common::intersection::Ray& ray, prev_test::common::intersection::RayCastResult& result)
{
    return prev_test::common::intersection::IntersectionTester::Intersects(ray, m_working, result);
}

void OBBBoundingVolumeComponent::Update(const glm::mat4& worldTransform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    prev::util::math::DecomposeTransform(worldTransform, rotation, translation, scale);

    m_working = prev_test::common::intersection::OBB{ rotation, m_original.position * scale + translation, m_original.GetHalfSize() * scale };

#ifdef RENDER_BOUNDING_VOLUMES
    m_model = BoundingVolumeModelFactory{ m_allocator }.CreateOBBModel(m_working, m_model);
#endif
}

BoundingVolumeType OBBBoundingVolumeComponent::GetType() const
{
    return BoundingVolumeType::OBB;
}

#ifdef RENDER_BOUNDING_VOLUMES
std::shared_ptr<prev_test::render::IModel> OBBBoundingVolumeComponent::GetModel() const
{
    return m_model;
}
#endif

prev_test::common::intersection::OBB OBBBoundingVolumeComponent::ScaleOBB(const prev_test::common::intersection::OBB& obb, const glm::vec3& scale)
{
    return prev_test::common::intersection::OBB{ obb.orientation, obb.position, obb.halfExtents * scale };
}

prev_test::common::intersection::OBB OBBBoundingVolumeComponent::OffsetOBB(const prev_test::common::intersection::OBB& obb, const glm::vec3& offset)
{
    return prev_test::common::intersection::OBB{ obb.orientation, obb.position + offset, obb.halfExtents };
}
} // namespace prev_test::component::ray_casting