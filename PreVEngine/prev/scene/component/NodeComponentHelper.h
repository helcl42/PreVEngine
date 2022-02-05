#ifndef __NODE_COMPONENT_HELPER_H__
#define __NODE_COMPONENT_HELPER_H__

#include "../../common/FlagSet.h"
#include "../../common/TagSet.h"
#include "../graph/GraphTraversal.h"
#include "ComponentRepository.h"

#include <memory>
#include <vector>
#include <sstream>

namespace prev::scene::component {
class NodeComponentHelper {
public:
    template <typename ComponentType>
    static std::shared_ptr<ComponentType> FindOne(const prev::common::TagSet& tagSet, const prev::scene::graph::LogicOperation operation = prev::scene::graph::LogicOperation::OR)
    {
        const auto node{ prev::scene::graph::GraphTraversal::Instance().FindOneWithTags(tagSet, operation) };
        if (node == nullptr) {
            std::stringstream ss;
            ss << tagSet;
            throw std::runtime_error("There is no such node with tags: " + ss.str());
        }
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > FindAll(const prev::common::TagSet& tagSet, const prev::scene::graph::LogicOperation operation = prev::scene::graph::LogicOperation::OR)
    {
        std::vector<std::shared_ptr<ComponentType> > resultComponents;
        const auto nodes{ prev::scene::graph::GraphTraversal::Instance().FindAllWithTags(tagSet, operation) };
        for (const auto& node : nodes) {
            const auto nodeComponents{ ComponentRepository<ComponentType>::Instance().GetAll(node->GetId()) };
            resultComponents.insert(resultComponents.end(), nodeComponents.cbegin(), nodeComponents.cend());
        }
        return resultComponents;
    }

    template <typename ComponentType>
    static void AddComponents(const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const std::vector<std::shared_ptr<ComponentType> >& components, const std::string& tag)
    {
        ComponentRepository<ComponentType>::Instance().Add(node->GetId(), components);

        auto tags = node->GetTags();
        tags.Add(tag);
        node->SetTags(tags);
    }

    template <typename ComponentType>
    static void AddComponent(const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const std::shared_ptr<ComponentType>& component, const std::string& tag)
    {
        AddComponents<ComponentType>(node, { component }, tag);
    }

    template <typename ComponentType>
    static void RemoveComponents(const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const std::string& tag)
    {
        ComponentRepository<ComponentType>::Instance().Remove(node->GetId());

        auto tags = node->GetTags();
        tags.Remove(tag);
        node->SetTags(tags);
    }

    template <typename ComponentType>
    static std::shared_ptr<ComponentType> GetComponent(const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
    {
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > GetComponents(const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
    {
        return ComponentRepository<ComponentType>::Instance().GetAll(node->GetId());
    }

    template <typename ComponentType>
    static bool HasComponent(const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
    {
        if (node) {
            return ComponentRepository<ComponentType>::Instance().Contains(node->GetId());
        }
        return false;
    }
};
} // namespace prev::scene::component

#endif