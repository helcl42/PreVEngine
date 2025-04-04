#ifndef __RAY_CASTER_OBSERVER_H__
#define __RAY_CASTER_OBSERVER_H__

#include "../../General.h"
#include "../../common/intersection/RayCastResult.h"
#include "../../component/ray_casting/RayCastingEvents.h"
#include "../../component/terrain/ITerrainComponent.h"

#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>

#include <optional>

namespace prev_test::scene::ray_casting {
class RayCastObserver final : public prev::scene::graph::SceneNode {
private:
    enum class IntersectionType {
        NONE,
        TERRAIN,
        OBJECT
    };

public:
    RayCastObserver();

    ~RayCastObserver() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    // Terrain
    std::optional<glm::vec3> FindTheClosestTerrainIntersection(const prev_test::common::intersection::Ray& ray) const;

    std::optional<glm::vec3> GetFirstPositionUnderAlongRay(const prev_test::common::intersection::Ray& ray) const;

    std::vector<glm::vec3> GenerateSegmentPositions(const prev_test::common::intersection::Ray& ray) const;

    std::optional<glm::vec3> BinarySearch(const uint32_t count, const float start, const float finish, const prev_test::common::intersection::Ray& ray) const;

    bool IntersectsInRange(const float start, const float finish, const prev_test::common::intersection::Ray& ray) const;

    bool IsUnderGround(const prev_test::component::terrain::ITerrainComponenet& terrain, const glm::vec3& testPoint, bool shouldReturn) const;

    std::shared_ptr<prev_test::component::terrain::ITerrainComponenet> GetTerrain(const glm::vec3& position) const;

    // Objects
    struct IntersectionNodeResult {
        prev_test::common::intersection::RayCastResult result{};
        std::shared_ptr<prev::scene::graph::ISceneNode> node{};
    };

    std::optional<IntersectionNodeResult> FindTheClosestIntersectingNode(const prev_test::common::intersection::Ray& ray) const;

    // Common
    void ResetAllSelectableNodes() const;

public:
    void operator()(const prev_test::component::ray_casting::RayEvent& rayEvt);

private:
    const uint32_t RECURSION_COUNT{ 200 };

    std::optional<prev_test::common::intersection::Ray> m_currentRay;

    prev::event::EventHandler<RayCastObserver, prev_test::component::ray_casting::RayEvent> m_rayHandler{ *this };
};
} // namespace prev_test::scene::ray_casting

#endif // !__RAY_CASTER_OBSERVER_H__
