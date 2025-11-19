#include "AABBBoundingVolumeComponent.h"

#ifdef RENDER_BOUNDING_VOLUMES
#include "BoundingVolumeModelFactory.h"
#endif

#include <prev/util/MathUtils.h>
#include <prev/util/intersection/IntersectionTester.h>

namespace prev_test::component::ray_casting {
AABBBoundingVolumeComponent::AABBBoundingVolumeComponent(prev::core::memory::Allocator& allocator, const prev::util::intersection::AABB& box, const glm::vec3& scale, const glm::vec3& offset)
    : m_allocator{ allocator }
    , m_scale{ scale }
    , m_offset{ offset }
{
    auto newBox = OffsetBox(box, offset);
    newBox = ScaleBox(newBox, scale);

    m_original = newBox;
    m_working = newBox;
    m_originalAABBPoints = newBox.GetPoints();
    m_vorkingAABBPoints = newBox.GetPoints();

#ifdef RENDER_BOUNDING_VOLUMES
    m_model = BoundingVolumeModelFactory{ m_allocator }.CreateAABBModel(m_working);
#endif
}

bool AABBBoundingVolumeComponent::IsInFrustum(const prev::util::intersection::Frustum& frustum)
{
    return prev::util::intersection::tester::Intersects(frustum, m_working);
}

bool AABBBoundingVolumeComponent::Intersects(const prev::util::intersection::Ray& ray, prev::util::intersection::RayCastResult& result)
{
    return prev::util::intersection::tester::Intersects(ray, m_working, result);
}

void AABBBoundingVolumeComponent::Update(const glm::mat4& worldTransform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    prev::util::math::DecomposeTransform(worldTransform, rotation, translation, scale);

    for (size_t i = 0; i < m_originalAABBPoints.size(); i++) {
        m_vorkingAABBPoints[i] = glm::scale(glm::mat4(1.0f), scale) * glm::mat4_cast(rotation) * glm::vec4(m_originalAABBPoints[i], 1.0f);
    }

    glm::vec3 minBound{ std::numeric_limits<float>::max() };
    glm::vec3 maxBound{ -std::numeric_limits<float>::max() };
    for (const auto& pt : m_vorkingAABBPoints) {
        minBound = glm::min(minBound, pt);
        maxBound = glm::max(maxBound, pt);
    }

    m_working = prev::util::intersection::AABB(glm::vec3(translation + minBound), glm::vec3(translation + maxBound));
#ifdef RENDER_BOUNDING_VOLUMES
    m_model = BoundingVolumeModelFactory{ m_allocator }.CreateAABBModel(m_working, m_model);
#endif
}

BoundingVolumeType AABBBoundingVolumeComponent::GetType() const
{
    return BoundingVolumeType::AABB;
}

#ifdef RENDER_BOUNDING_VOLUMES
std::shared_ptr<prev_test::render::IModel> AABBBoundingVolumeComponent::GetModel() const
{
    return m_model;
}
#endif

prev::util::intersection::AABB AABBBoundingVolumeComponent::ScaleBox(const prev::util::intersection::AABB& box, const glm::vec3& scale)
{
    return prev::util::intersection::AABB{ box.minExtents * scale, box.maxExtents * scale };
}

prev::util::intersection::AABB AABBBoundingVolumeComponent::OffsetBox(const prev::util::intersection::AABB& box, const glm::vec3& offset)
{
    return prev::util::intersection::AABB{ box.minExtents + offset, box.maxExtents + offset };
}
} // namespace prev_test::component::ray_casting