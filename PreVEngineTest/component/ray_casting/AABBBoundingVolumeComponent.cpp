#include "AABBBoundingVolumeComponent.h"
#include "../../common/intersection/IntersectionTester.h"
#include "BoundingVolumeModelFactory.h"

#include <prev/util/MathUtils.h>

namespace prev_test::component::ray_casting {
AABBBoundingVolumeComponent::AABBBoundingVolumeComponent(const prev_test::common::intersection::AABB& box, const float scale, const glm::vec3& offset)
    : m_scale(scale)
    , m_offset(offset)
{
    prev_test::common::intersection::AABB newBox = OffsetBox(box, offset);
    newBox = ScaleBox(newBox, scale);

    m_original = newBox;
    m_working = newBox;
    m_originalAABBPoints = newBox.GetPoints();
    m_vorkingAABBPoints = newBox.GetPoints();
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
    const auto rotationScaleTransform = prev::util::MathUtil::ExtractRotation(worldTransform);
    const auto translation = prev::util::MathUtil::ExtractTranslation(worldTransform);

    for (auto i = 0; i < m_originalAABBPoints.size(); i++) {
        m_vorkingAABBPoints[i] = rotationScaleTransform * glm::vec4(m_originalAABBPoints[i], 1.0f);
    }

    glm::vec3 minBound{ std::numeric_limits<float>::max() };
    glm::vec3 maxBound{ std::numeric_limits<float>::min() };
    for (const auto pt : m_vorkingAABBPoints) {
        for (auto i = 0; i < minBound.length(); i++) {
            minBound[i] = std::min(minBound[i], pt[i]);
            maxBound[i] = std::max(maxBound[i], pt[i]);
        }
    }

    m_working = prev_test::common::intersection::AABB(glm::vec3(translation + minBound), glm::vec3(translation + maxBound));
#ifdef RENDER_BOUNDING_VOLUMES
    BoundingVolumeModelFactory modelFactory{};
    m_model = modelFactory.CreateAABBModel(m_working);
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

prev_test::common::intersection::AABB AABBBoundingVolumeComponent::ScaleBox(const prev_test::common::intersection::AABB& box, const float scale)
{
    prev_test::common::intersection::AABB result{};
    result.minExtents = box.minExtents * scale;
    result.maxExtents = box.maxExtents * scale;
    return result;
}

prev_test::common::intersection::AABB AABBBoundingVolumeComponent::OffsetBox(const prev_test::common::intersection::AABB& box, const glm::vec3& offset)
{
    prev_test::common::intersection::AABB result{};
    result.minExtents = box.minExtents + offset;
    result.maxExtents = box.maxExtents + offset;
    return result;
}
} // namespace prev_test::component::ray_casting