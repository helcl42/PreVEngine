#ifndef __NODE_COMPONENT_HELPER_H__
#define __NODE_COMPONENT_HELPER_H__

#include "ComponentRepository.h"

#include "../graph/GraphTraversal.h"
#include "../graph/ISceneNode.h"

#include "../../common/FlagSet.h"
#include "../../common/TagSet.h"

#include <memory>
#include <sstream>
#include <vector>

namespace prev::scene::component {
class NodeComponentHelper final {
public:
    template <typename ComponentType>
    static std::shared_ptr<ComponentType> FindOne(const std::shared_ptr<graph::ISceneNode>& root, const prev::common::TagSet& tagSet, const prev::scene::graph::LogicOperation operation = prev::scene::graph::LogicOperation::OR)
    {
        const auto node{ prev::scene::graph::GraphTraversal::FindOneWithTags(root, tagSet, operation) };
        if (!node) {
            throw std::runtime_error("There is no such node with tags: " + tagSet.ToString());
        }

        auto component{ node->GetComponentRepository().FindOne<ComponentType>() };
        if (!component) {
            throw std::runtime_error("Component with tags = " + tagSet.ToString() + " does not exist in node id = " + std::to_string(node->GetId()) + ", tags = " + node->GetTags().ToString());
        }

        return component;
    }

    template <typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType>> FindAll(const std::shared_ptr<graph::ISceneNode>& root, const prev::common::TagSet& tagSet, const prev::scene::graph::LogicOperation operation = prev::scene::graph::LogicOperation::OR)
    {
        std::vector<std::shared_ptr<ComponentType>> resultComponents;
        const auto nodes{ prev::scene::graph::GraphTraversal::FindAllWithTags(root, tagSet, operation) };
        for (const auto& node : nodes) {
            const auto nodeComponents{ node->GetComponentRepository().GetAll<ComponentType>() };
            resultComponents.insert(resultComponents.end(), nodeComponents.cbegin(), nodeComponents.cend());
        }
        return resultComponents;
    }

    template <typename ComponentType>
    static void AddComponent(const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const std::shared_ptr<ComponentType>& component, const prev::common::TagSet& extraTagSet = {})
    {
        node->GetComponentRepository().Add(component);
        node->GetTags() += extraTagSet;
    }

    template <typename ComponentType>
    static void AddComponents(const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const std::vector<std::shared_ptr<ComponentType>>& components, const prev::common::TagSet& extraTagSet = {})
    {
        node->GetComponentRepository().Add<ComponentType>(components);
        node->GetTags() += extraTagSet;
    }

    template <typename ComponentType>
    static void RemoveComponents(const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev::common::TagSet& extraTagSet = {})
    {
        node->GetComponentRepository().Remove<ComponentType>();
        node->GetTags() -= extraTagSet;
    }

    template <typename ComponentType>
    static std::shared_ptr<ComponentType> GetComponent(const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
    {
        auto component{ node->GetComponentRepository().FindOne<ComponentType>() };
        if (!component) {
            throw std::runtime_error("Could not get component " + std::string(std::type_index(typeid(ComponentType)).name()) + " for node id = " + std::to_string(node->GetId()) + ", tags = " + node->GetTags().ToString());
        }

        return component;
    }

    template <typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType>> GetComponents(const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
    {
        const auto components{ node->GetComponentRepository().FindAll<ComponentType>() };
        if (components.empty()) {
            throw std::runtime_error("Could not get components for node id = " + std::to_string(node->GetId()) + ", tags = " + node->GetTags().ToString());
        }

        return components;
    }

    template <typename ComponentType>
    static bool HasComponent(const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
    {
        if (node) {
            const auto component{ node->GetComponentRepository().FindOne<ComponentType>() };
            if (component) {
                return true;
            }
        }
        return false;
    }

    template <typename ComponentType>
    static std::shared_ptr<ComponentType> FindComponent(const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
    {
        return node->GetComponentRepository().FindOne<ComponentType>();
    }
};
} // namespace prev::scene::component

#endif