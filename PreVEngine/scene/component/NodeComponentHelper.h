#ifndef __NODE_COMPONENT_HELPER_H__
#define __NODE_COMPONENT_HELPER_H__

#include "../../common/FlagSet.h"
#include "../../common/TagSet.h"
#include "../graph/GraphTraversal.h"

#include <memory>
#include <vector>

namespace prev {
class NodeComponentHelper {
public:
    template <typename NodeFlagsType, typename ComponentType>
    static std::shared_ptr<ComponentType> FindOne(const prev::common::TagSet& tagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto node = GraphTraversal<NodeFlagsType>::Instance().FindOneWithTags(tagSet, operation);
        if (node == nullptr) {
            throw std::runtime_error("There is no such node..");
        }
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::shared_ptr<ComponentType> FindOne(const prev::common::FlagSet<NodeFlagsType>& flagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto node = GraphTraversal<NodeFlagsType>::Instance().FindOneWithFlags(flagSet, operation);
        if (node == nullptr) {
            throw std::runtime_error("There is no such node..");
        }
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > FindAll(const prev::common::TagSet& tagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto nodes = GraphTraversal<NodeFlagsType>::Instance().FindAllWithTags(tagSet, operation);

        std::vector<std::shared_ptr<ComponentType> > resultComponents(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            resultComponents[i] = ComponentRepository<ComponentType>::Instance().Get(nodes[i]->GetId());
        }
        return resultComponents;
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > FindAll(const prev::common::FlagSet<NodeFlagsType>& flagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto nodes = GraphTraversal<NodeFlagsType>::Instance().FindAllWithFlags(flagSet, operation);

        std::vector<std::shared_ptr<ComponentType> > resultComponents(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            resultComponents[i] = ComponentRepository<ComponentType>::Instance().Get(nodes[i]->GetId());
        }
        return resultComponents;
    }

    template <typename FlagType, typename ComponentType>
    static void AddComponent(const std::shared_ptr<ISceneNode<FlagType> >& node, const std::shared_ptr<ComponentType>& component, const FlagType flag)
    {
        ComponentRepository<ComponentType>::Instance().Add(node->GetId(), std::move(component));

        auto flags = node->GetFlags();
        flags.Set(flag, true);
        node->SetFlags(flags);
    }

    template <typename FlagType, typename ComponentType>
    static void RemoveComponent(const std::shared_ptr<ISceneNode<FlagType> >& node, const FlagType flag)
    {
        const auto component = ComponentRepository<ComponentType>::Instance().Get(node->GetId());

        ComponentRepository<ComponentType>::Instance().Remove(node->GetId());

        auto flags = node->GetFlags();
        flags.Set(flag, false);
        node->SetFlags(flags);
    }

    template <typename FlagType, typename ComponentType>
    static std::shared_ptr<ComponentType> GetComponent(const std::shared_ptr<ISceneNode<FlagType> >& node)
    {
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename FlagType, typename ComponentType>
    static bool HasComponent(const std::shared_ptr<ISceneNode<FlagType> >& node)
    {
        if (node) {
            return ComponentRepository<ComponentType>::Instance().Contains(node->GetId());
        }
        return false;
    }
};
} // namespace prev

#endif