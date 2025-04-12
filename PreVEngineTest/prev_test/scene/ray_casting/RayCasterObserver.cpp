#include "RayCasterObserver.h"

#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/ray_casting/ISelectableComponent.h"
#include "../../component/terrain/ITerrainManagerComponent.h"

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
    if (m_currentRay.has_value()) {
        ResetAllSelectableNodes();

        IntersectionType intersectionType{ IntersectionType::NONE };
        const auto& currentRayValue{ m_currentRay.value() };
        const auto currentTerrainIntersectionPoint{ FindTheClosestTerrainIntersection(currentRayValue) };
        const auto closestIntersectingObject{ FindTheClosestIntersectingNode(currentRayValue) };
        if (currentTerrainIntersectionPoint.has_value() && closestIntersectingObject.has_value()) {
            const float terrainRayLength{ glm::distance(currentTerrainIntersectionPoint.value(), currentRayValue.origin) };
            const float objectRayLength{ closestIntersectingObject.value().result.t };
            intersectionType = terrainRayLength < objectRayLength ? IntersectionType::TERRAIN : IntersectionType::OBJECT;
        } else if (currentTerrainIntersectionPoint.has_value()) {
            intersectionType = IntersectionType::TERRAIN;
        } else if (closestIntersectingObject.has_value()) {
            intersectionType = IntersectionType::OBJECT;
        }

        if (intersectionType == IntersectionType::TERRAIN) {
            auto terrainManagerNode{ prev::scene::graph::GraphTraversal::FindOneWithTags(GetRoot(), { TAG_TERRAIN_MANAGER_COMPONENT, TAG_SELECTABLE_COMPONENT }, prev::scene::graph::LogicOperation::AND) };
            auto selectableComponent{ prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::ray_casting::ISelectableComponent>(terrainManagerNode) };
            selectableComponent->SetSelected(true);
            selectableComponent->SetPosition(currentTerrainIntersectionPoint.value());
        } else if (intersectionType == IntersectionType::OBJECT) {
            const auto& node{ closestIntersectingObject.value().node };
            const auto& rayCastResult{ closestIntersectingObject.value().result };
            auto selectableComponent{ prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::ray_casting::ISelectableComponent>(node) };
            selectableComponent->SetSelected(true);
            selectableComponent->SetPosition(rayCastResult.point);
        }
    }

    SceneNode::Update(deltaTime);
}

void RayCastObserver::ShutDown()
{
    SceneNode::ShutDown();
}

std::optional<glm::vec3> RayCastObserver::FindTheClosestTerrainIntersection(const prev_test::common::intersection::Ray& ray) const
{
    std::optional<glm::vec3> currentTerrainIntersectionPoint{};
    if (IntersectsInRange(0.0f, ray.length, ray)) {
        currentTerrainIntersectionPoint = BinarySearch(0, 0, ray.length, ray);
    } else {
        currentTerrainIntersectionPoint = GetFirstPositionUnderAlongRay(ray);
    }
    return currentTerrainIntersectionPoint;
}

std::optional<glm::vec3> RayCastObserver::GetFirstPositionUnderAlongRay(const prev_test::common::intersection::Ray& ray) const
{
    const auto segmentPositions{ GenerateSegmentPositions(ray) };
    for (const auto& segmentPosition : segmentPositions) {
        const auto terrain{ GetTerrain(segmentPosition) };
        if (terrain != nullptr) {
            float currentTerrainHeight{ 0.0f };
            if (!terrain->GetHeightAt(segmentPosition, currentTerrainHeight)) {
                continue;
            }
            if (segmentPosition.y < currentTerrainHeight) {
                return glm::vec3{ segmentPosition.x, currentTerrainHeight, segmentPosition.z };
            }
        }
    }
    return {};
}

std::vector<glm::vec3> RayCastObserver::GenerateSegmentPositions(const prev_test::common::intersection::Ray& ray) const
{
    const float distanceBetweenNodes{ ray.length / RECURSION_COUNT };
    std::vector<glm::vec3> result;
    float currentDistance{ 0.0f };
    while (currentDistance < ray.length) {
        const glm::vec3 newNodePoint{ ray.GetPointAtDistances(currentDistance) };
        result.push_back(newNodePoint);
        currentDistance += distanceBetweenNodes;
    }
    return result;
}

std::optional<glm::vec3> RayCastObserver::BinarySearch(const uint32_t count, const float start, const float finish, const prev_test::common::intersection::Ray& ray) const
{
    const float half{ start + ((finish - start) / 2.0f) };
    if (count >= RECURSION_COUNT) {
        const glm::vec3 endPoint{ ray.GetPointAtDistances(half) };
        const auto terrain{ GetTerrain(endPoint) };
        if (terrain != nullptr) {
            return endPoint;
        } else {
            return {};
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
    const glm::vec3 startPoint{ ray.GetPointAtDistances(start) };
    const glm::vec3 endPoint{ ray.GetPointAtDistances(finish) };

    const auto terrainAtStart{ GetTerrain(startPoint) };
    if (!terrainAtStart) {
        return false;
    }

    const auto terrainAtEnd{ GetTerrain(endPoint) };
    if (!terrainAtEnd) {
        return false;
    }

    if (!IsUnderGround(*terrainAtStart, startPoint, false) && IsUnderGround(*terrainAtEnd, endPoint, true)) {
        return true;
    }
    return false;
}

bool RayCastObserver::IsUnderGround(const prev_test::component::terrain::ITerrainComponent& terrain, const glm::vec3& testPoint, bool shouldReturn) const
{
    float height{ 0.0f };
    if (!terrain.GetHeightAt(testPoint, height)) {
        return !shouldReturn;
    }
    if (testPoint.y < height) {
        return true;
    }
    return false;
}

std::shared_ptr<prev_test::component::terrain::ITerrainComponent> RayCastObserver::GetTerrain(const glm::vec3& position) const
{
    const auto terrainManager{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::terrain::ITerrainManagerComponent>(GetRoot(), { TAG_TERRAIN_MANAGER_COMPONENT }) };
    return terrainManager->GetTerrainAt(position);
}

std::optional<RayCastObserver::IntersectionNodeResult> RayCastObserver::FindTheClosestIntersectingNode(const prev_test::common::intersection::Ray& ray) const
{
    std::optional<IntersectionNodeResult> theClosestNode;
    float minDistance{ std::numeric_limits<float>::max() };
    auto selectableNodes{ prev::scene::graph::GraphTraversal::FindAllWithTags(GetRoot(), { TAG_SELECTABLE_COMPONENT, TAG_BOUNDING_VOLUME_COMPONENT }, prev::scene::graph::LogicOperation::AND) };
    for (const auto& selectable : selectableNodes) {
        const auto boundingVolume{ prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(selectable) };
        prev_test::common::intersection::RayCastResult rayCastResult{};
        if (boundingVolume->Intersects(ray, rayCastResult)) {
            if (rayCastResult.t < minDistance) {
                theClosestNode = { rayCastResult, selectable };
                minDistance = rayCastResult.t;
            }
        }
    }
    return theClosestNode;
}

void RayCastObserver::ResetAllSelectableNodes() const
{
    auto selectableNodes{ prev::scene::graph::GraphTraversal::FindAllWithTags(GetRoot(), { TAG_SELECTABLE_COMPONENT }) };
    for (auto& selectableNode : selectableNodes) {
        auto selectableComponent{ prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::ray_casting::ISelectableComponent>(selectableNode) };
        selectableComponent->Reset();
    }
}

void RayCastObserver::operator()(const prev_test::component::ray_casting::RayEvent& rayEvt)
{
    m_currentRay = prev_test::common::intersection::Ray{ rayEvt.ray };
}
} // namespace prev_test::scene::ray_casting