#ifndef __NODE_COMPONENT_HELPER_H__
#define __NODE_COMPONENT_HELPER_H__

#include "../../common/FlagSet.h"
#include "../../common/TagSet.h"
#include "../graph/GraphTraversal.h"
#include "ComponentRepository.h"

#include <memory>
#include <vector>

namespace prev::scene::component {
class NodeComponentHelper {
public:
    template <typename ComponentType>
    static std::shared_ptr<ComponentType> FindOne(const prev::common::TagSet& tagSet, const prev::scene::graph::LogicOperation operation = prev::scene::graph::LogicOperation::OR)
    {
        const auto node = prev::scene::graph::GraphTraversal::Instance().FindOneWithTags(tagSet, operation);
        if (node == nullptr) {
            throw std::runtime_error("There is no such node..");
        }
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > FindAll(const prev::common::TagSet& tagSet, const prev::scene::graph::LogicOperation operation = prev::scene::graph::LogicOperation::OR)
    {
        const auto nodes = prev::scene::graph::GraphTraversal::Instance().FindAllWithTags(tagSet, operation);

        std::vector<std::shared_ptr<ComponentType> > resultComponents(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            resultComponents[i] = ComponentRepository<ComponentType>::Instance().Get(nodes[i]->GetId());
        }
        return resultComponents;
    }

    template <typename ComponentType>
    static void AddComponent(const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const std::shared_ptr<ComponentType>& component, const std::string& tag)
    {
        ComponentRepository<ComponentType>::Instance().Add(node->GetId(), std::move(component));

        auto tags = node->GetTags();
        tags.Add(tag);
        node->SetTags(tags);
    }

    template <typename ComponentType>
    static void RemoveComponent(const std::shared_ptr<prev::scene::graph::ISceneNode >& node, const std::string& tag)
    {
        const auto component = ComponentRepository<ComponentType>::Instance().Get(node->GetId());

        ComponentRepository<ComponentType>::Instance().Remove(node->GetId());

        auto tags = node->GetTags();
        tags.Remove(tag);
        node->SetTags(tags);
    }

    template <typename ComponentType>
    static std::shared_ptr<ComponentType> GetComponent(const std::shared_ptr<prev::scene::graph::ISceneNode >& node)
    {
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename ComponentType>
    static bool HasComponent(const std::shared_ptr<prev::scene::graph::ISceneNode >& node)
    {
        if (node) {
            return ComponentRepository<ComponentType>::Instance().Contains(node->GetId());
        }
        return false;
    }
};
} // namespace prev::scene::component

#endif