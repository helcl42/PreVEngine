#include "RayCasterObserver.h"
#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/ray_casting/ISelectableComponent.h"
#include "../../component/terrain/ITerrainManagerComponent.h"

#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::ray_casting {
RayCastObserver::RayCastObserver()
    : SceneNode()
{
}

void RayCastObserver::Init()
{
    SceneNode::Init();
}

void RayCastObserver::Update(float deltaTime)
{
    if (!m_currentRay.IsNull()) {

        // reset any selectable components
        auto selectableNodes = GetSelectableNodes();
        for (auto selectableNode : selectableNodes) {
            auto selectableComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::ISelectableComponent>::Instance().Get(selectableNode->GetId());
            selectableComponent->Reset();
        }

        IntersectionType intersectionType{ IntersectionType::NONE };
        const auto& currentRayValue = m_currentRay.GetValue();
        const auto currentTerrainIntersectionPoint = FindTheClosestTerrainIntersection(currentRayValue);
        const auto closestIntersectingObject = FindTheClosestIntersectingNode(currentRayValue);
        if (!currentTerrainIntersectionPoint.IsNull() && !closestIntersectingObject.IsNull()) {
            const float terrainRayLength = glm::distance(currentTerrainIntersectionPoint.GetValue(), currentRayValue.origin);
            const float objectRayLength = std::get<1>(closestIntersectingObject.GetValue()).t;
            if (terrainRayLength < objectRayLength) {
                intersectionType = IntersectionType::TERRAIN;
            } else {
                intersectionType = IntersectionType::OBJECT;
            }
        } else {
            if (!currentTerrainIntersectionPoint.IsNull()) {
                intersectionType = IntersectionType::TERRAIN;
            } else if (!closestIntersectingObject.IsNull()) {
                intersectionType = IntersectionType::OBJECT;
            }
        }

        if (intersectionType == IntersectionType::TERRAIN) {
            auto terrainManagerNode = prev::scene::graph::GraphTraversal::Instance().FindOneWithTags({ TAG_TERRAIN_MANAGER_COMPONENT, TAG_SELECTABLE_COMPONENT }, prev::scene::graph::LogicOperation::AND);
            auto selectableComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::ISelectableComponent>::Instance().Get(terrainManagerNode->GetId());
            selectableComponent->SetSelected(true);
            selectableComponent->SetPosition(currentTerrainIntersectionPoint.GetValue());
        } else if (intersectionType == IntersectionType::OBJECT) {
            const auto node = std::get<0>(closestIntersectingObject.GetValue());
            const auto& rayCastResult = std::get<1>(closestIntersectingObject.GetValue());
            auto selectableComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::ISelectableComponent>::Instance().Get(node->GetId());
            selectableComponent->SetSelected(true);
            selectableComponent->SetPosition(rayCastResult.point);
        }

        // TODO
        // render node collision points -> it is stored in selectableComponent
    }

    SceneNode::Update(deltaTime);
}

void RayCastObserver::ShutDown()
{
    SceneNode::ShutDown();
}

prev::common::pattern::Nullable<glm::vec3> RayCastObserver::FindTheClosestTerrainIntersection(const prev_test::common::intersection::Ray& ray) const
{
    prev::common::pattern::Nullable<glm::vec3> currentTerrainIntersectionPoint{};
    if (IntersectsInRange(0.0f, ray.length, ray)) {
        currentTerrainIntersectionPoint = BinarySearch(0, 0, ray.length, ray);
    } else {
        currentTerrainIntersectionPoint = GetFirstPositionUnderAlongRay(ray);
    }
    return currentTerrainIntersectionPoint;
}

prev::common::pattern::Nullable<glm::vec3> RayCastObserver::GetFirstPositionUnderAlongRay(const prev_test::common::intersection::Ray& ray) const
{
    const auto segmentPositions = GenerateSegmentPositions(ray);
    for (const auto& segmentPosition : segmentPositions) {
        const auto terrain = GetTerrain(segmentPosition);
        if (terrain != nullptr) {
            float currentTerrainHeight = 0.0f;
            if (!terrain->GetHeightAt(segmentPosition, currentTerrainHeight)) {
                continue;
            }
            if (segmentPosition.y < currentTerrainHeight) {
                return prev::common::pattern::Nullable<glm::vec3>({ segmentPosition.x, currentTerrainHeight, segmentPosition.z });
            }
        }
    }
    return prev::common::pattern::Nullable<glm::vec3>();
}

std::vector<glm::vec3> RayCastObserver::GenerateSegmentPositions(const prev_test::common::intersection::Ray& ray) const
{
    const float distanceBetweenNodes = ray.length / RECURSION_COUNT;
    const glm::vec3 start = ray.GetStartPoint();
    const glm::vec3 end = ray.GetEndPoint();

    std::vector<glm::vec3> result;
    float currentDistance = 0.0f;
    while (currentDistance < ray.length) {
        glm::vec3 newNodePoint = ray.GetPointAtDistances(currentDistance);
        result.push_back(newNodePoint);
        currentDistance += distanceBetweenNodes;
    }
    return result;
}

prev::common::pattern::Nullable<glm::vec3> RayCastObserver::BinarySearch(const uint32_t count, const float start, const float finish, const prev_test::common::intersection::Ray& ray) const
{
    const float half = start + ((finish - start) / 2.0f);
    if (count >= RECURSION_COUNT) {
        const glm::vec3 endPoint = ray.GetPointAtDistances(half);
        const auto terrain = GetTerrain(endPoint);
        if (terrain != nullptr) {
            return prev::common::pattern::Nullable<glm::vec3>(endPoint);
        } else {
            return prev::common::pattern::Nullable<glm::vec3>();
        }
    }

    if (IntersectsInRange(start, half, ray)) {
        return BinarySearch(count + 1, start, half, ray);
    } else {
        return BinarySearch(count + 1, half, finish, ray);
    }
}

bool RayCastObserver::IntersectsInRange(const float start, const float finish, const prev_test::common::intersection::Ray& ray) const
{
    const glm::vec3 startPoint = ray.GetPointAtDistances(start);
    const glm::vec3 endPoint = ray.GetPointAtDistances(finish);

    const auto terrainAtStart = GetTerrain(startPoint);
    if (!terrainAtStart) {
        return false;
    }

    const auto terrainAtEnd = GetTerrain(endPoint);
    if (!terrainAtEnd) {
        return false;
    }

    if (!IsUnderGround(terrainAtStart, startPoint, false) && IsUnderGround(terrainAtEnd, endPoint, true)) {
        return true;
    }
    return false;
}

bool RayCastObserver::IsUnderGround(const std::shared_ptr<prev_test::component::terrain::ITerrainComponenet>& terrain, const glm::vec3& testPoint, bool shouldReturn) const
{
    float height = 0.0f;
    if (!terrain->GetHeightAt(testPoint, height)) {
        return !shouldReturn;
    }
    if (testPoint.y < height) {
        return true;
    }
    return false;
}

std::shared_ptr<prev_test::component::terrain::ITerrainComponenet> RayCastObserver::GetTerrain(const glm::vec3& position) const
{
    const auto terrainManager = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::terrain::ITerrainManagerComponent>({ TAG_TERRAIN_MANAGER_COMPONENT });
    return terrainManager->GetTerrainAt(position);
}

std::vector<std::shared_ptr<prev::scene::graph::ISceneNode> > RayCastObserver::GetSelectableNodes() const
{
    return prev::scene::graph::GraphTraversal::Instance().FindAllWithTags({ TAG_SELECTABLE_COMPONENT });
}

prev::common::pattern::Nullable<std::tuple<std::shared_ptr<prev::scene::graph::ISceneNode>, prev_test::common::intersection::RayCastResult> > RayCastObserver::FindTheClosestIntersectingNode(const prev_test::common::intersection::Ray& ray) const
{
    prev::common::pattern::Nullable<std::tuple<std::shared_ptr<ISceneNode>, prev_test::common::intersection::RayCastResult> > theClosestNode;
    float minDistance = std::numeric_limits<float>::max();

    auto selectableNodes = prev::scene::graph::GraphTraversal::Instance().FindAllWithTags({ TAG_SELECTABLE_COMPONENT, TAG_BOUNDING_VOLUME_COMPONENT }, prev::scene::graph::LogicOperation::AND);
    for (auto selectable : selectableNodes) {
        const auto boundingVolume = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(selectable->GetId());

        prev_test::common::intersection::RayCastResult rayCastResult{};
        if (boundingVolume->Intersects(ray, rayCastResult)) {
            if (rayCastResult.t < minDistance) {
                theClosestNode = prev::common::pattern::Nullable<std::tuple<std::shared_ptr<ISceneNode>, prev_test::common::intersection::RayCastResult> >({ selectable, rayCastResult });
                minDistance = rayCastResult.t;
            }
        }
    }
    return theClosestNode;
}

void RayCastObserver::operator()(const prev_test::component::ray_casting::RayEvent& rayEvt)
{
    m_currentRay = prev::common::pattern::Nullable<prev_test::common::intersection::Ray>{ rayEvt.ray };
}
} // namespace prev_test::scene::ray_casting