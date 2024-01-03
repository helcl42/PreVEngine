#include "AABBBoundingVolumeComponent.h"

#include "../../common/intersection/IntersectionTester.h"

#ifdef RENDER_BOUNDING_VOLUMES
#include "BoundingVolumeModelFactory.h"
#endif

#include <prev/util/MathUtils.h>

namespace prev_test::component::ray_casting {
AABBBoundingVolumeComponent::AABBBoundingVolumeComponent(const prev_test::common::intersection::AABB& box, const glm::vec3& scale, const glm::vec3& offset)
    : m_scale(scale)
    , m_offset(offset)
{
    auto newBox = OffsetBox(box, offset);
    newBox = ScaleBox(newBox, scale);

    m_original = newBox;
    m_working = newBox;
    m_originalAABBPoints = newBox.GetPoints();
    m_vorkingAABBPoints = newBox.GetPoints();

#ifdef RENDER_BOUNDING_VOLUMES
    m_model = BoundingVolumeModelFactory{}.CreateAABBModel(m_working);
#endif
}

bool AABBBoundingVolumeComponent::IsInFrustum(const prev_test::common::intersection::Frustum& frustum)
{
    return prev_test::common::intersection::IntersectionTester::Intersects(frustum, m_working);
}

bool AABBBoundingVolumeComponent::Intersects(const prev_test::common::intersection::Ray& ray, prev_test::common::intersection::RayCastResult& result)
{
    return prev_test::common::intersection::IntersectionTester::Intersects(ray, m_working, result);
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
    glm::vec3 maxBound{ std::numeric_limits<float>::min() };
    for (const auto& pt : m_vorkingAABBPoints) {
        for (int i = 0; i < minBound.length(); i++) {
            minBound[i] = std::min(minBound[i], pt[i]);
            maxBound[i] = std::max(maxBound[i], pt[i]);
        }
    }

    m_working = prev_test::common::intersection::AABB(glm::vec3(translation + minBound), glm::vec3(translation + maxBound));
#ifdef RENDER_BOUNDING_VOLUMES
    m_model = BoundingVolumeModelFactory{}.CreateAABBModel(m_working, m_model);
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

prev_test::common::intersection::AABB AABBBoundingVolumeComponent::ScaleBox(const prev_test::common::intersection::AABB& box, const glm::vec3& scale)
{
    return prev_test::common::intersection::AABB{ box.minExtents * scale, box.maxExtents * scale };
}

prev_test::common::intersection::AABB AABBBoundingVolumeComponent::OffsetBox(const prev_test::common::intersection::AABB& box, const glm::vec3& offset)
{
    return prev_test::common::intersection::AABB{ box.minExtents + offset, box.maxExtents + offset };
}
} // namespace prev_test::component::ray_casting