#ifndef __RAY_CASTER_OBSERVER_H__
#define __RAY_CASTER_OBSERVER_H__

#include "RayCasterEvents.h"

#include "../../component/terrain/ITerrainComponent.h"

#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>
#include <prev/util/intersection/RayCastResult.h>

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
    std::optional<glm::vec3> FindTheClosestTerrainIntersection(const prev::util::intersection::Ray& ray) const;

    std::optional<glm::vec3> GetFirstPositionUnderAlongRay(const prev::util::intersection::Ray& ray) const;

    std::vector<glm::vec3> GenerateSegmentPositions(const prev::util::intersection::Ray& ray) const;

    std::optional<glm::vec3> BinarySearch(const uint32_t count, const float start, const float finish, const prev::util::intersection::Ray& ray) const;

    bool IntersectsInRange(const float start, const float finish, const prev::util::intersection::Ray& ray) const;

    bool IsUnderGround(const prev_test::component::terrain::ITerrainComponent& terrain, const glm::vec3& testPoint, bool shouldReturn) const;

    std::shared_ptr<prev_test::component::terrain::ITerrainComponent> GetTerrain(const glm::vec3& position) const;

    // Objects
    struct IntersectionNodeResult {
        prev::util::intersection::RayCastResult result{};
        std::shared_ptr<prev::scene::graph::ISceneNode> node{};
    };

    std::optional<IntersectionNodeResult> FindTheClosestIntersectingNode(const prev::util::intersection::Ray& ray) const;

    // Common
    void ResetAllSelectableNodes() const;

public:
    void operator()(const RayEvent& rayEvt);

private:
    const uint32_t RECURSION_COUNT{ 200 };

    std::optional<prev::util::intersection::Ray> m_currentRay;

    prev::event::EventHandler<RayCastObserver, RayEvent> m_rayHandler{ *this };
};
} // namespace prev_test::scene::ray_casting

#endif // !__RAY_CASTER_OBSERVER_H__
