#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Common.h"
#include "SceneGraph.h"
#include "Events.h"

#include <map>

namespace PreVEngine {
template <typename ItemType>
class ComponentRepository final : public Singleton<ComponentRepository<ItemType> > {
public:
    ~ComponentRepository() = default;

public:
    std::shared_ptr<ItemType> Get(const uint64_t id) const
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " does not exist in this repository.");
        }

        return m_components.at(id);
    }

    void Add(const uint64_t id, const std::shared_ptr<ItemType>& component)
    {
        if (Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " already exist in this repository.");
        }

        m_components[id] = component;
    }

    void Remove(const uint64_t id)
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " does not exist in this repository.");
        }

        m_components.erase(id);
    }

    bool Contains(const uint64_t id) const
    {
        return m_components.find(id) != m_components.cend();
    }

    void operator()(const SceneNodeShutDownEvent& evt)
    {
        if (Contains(evt.id)) {
            Remove(evt.id);
        }
    }

private:
    ComponentRepository() = default;

private:
    friend class Singleton<ComponentRepository<ItemType> >;

private:
    std::map<uint64_t, std::shared_ptr<ItemType> > m_components;

    EventHandler<ComponentRepository<ItemType>, SceneNodeShutDownEvent> m_shutDownHandler{ *this };
};

class NodeComponentHelper {
public:
    template <typename NodeFlagsType, typename ComponentType>
    static std::shared_ptr<ComponentType> FindOne(const TagSet& tagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto node = GraphTraversal<NodeFlagsType>::Instance().FindOneWithTags(tagSet, operation);
        if (node == nullptr) {
            throw std::runtime_error("There is no such node..");
        }
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::shared_ptr<ComponentType> FindOne(const FlagSet<NodeFlagsType>& flagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto node = GraphTraversal<NodeFlagsType>::Instance().FindOneWithFlags(flagSet, operation);
        if (node == nullptr) {
            throw std::runtime_error("There is no such node..");
        }
        return ComponentRepository<ComponentType>::Instance().Get(node->GetId());
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > FindAll(const TagSet& tagSet, const LogicOperation operation = LogicOperation::OR)
    {
        const auto nodes = GraphTraversal<NodeFlagsType>::Instance().FindAllWithTags(tagSet, operation);

        std::vector<std::shared_ptr<ComponentType> > resultComponents(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            resultComponents[i] = ComponentRepository<ComponentType>::Instance().Get(nodes[i]->GetId());
        }
        return resultComponents;
    }

    template <typename NodeFlagsType, typename ComponentType>
    static std::vector<std::shared_ptr<ComponentType> > FindAll(const FlagSet<NodeFlagsType>& flagSet, const LogicOperation operation = LogicOperation::OR)
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
} // namespace PreVEngine

#endif