#ifndef __GRAPH_TRAVERSAL_H__
#define __GRAPH_TRAVERSAL_H__

#include "../../common/pattern/Singleton.h"
#include "ISceneNode.h"

namespace prev::scene::graph {
enum class LogicOperation {
    OR,
    AND
};

template <typename NodeFlagsType>
class GraphTraversal final : public prev::common::pattern::Singleton<GraphTraversal<NodeFlagsType> > {
public:
    ~GraphTraversal() = default;

private:
    bool HasFlags(const std::shared_ptr<ISceneNode<NodeFlagsType> >& node, const prev::common::FlagSet<NodeFlagsType>& flagsToCheck, const LogicOperation operation) const
    {
        if (operation == LogicOperation::AND) {
            return node->GetFlags().HasAll(flagsToCheck);
        } else if (operation == LogicOperation::OR) {
            return node->GetFlags().HasAny(flagsToCheck);
        }
        return false;
    }

    bool HasTags(const std::shared_ptr<ISceneNode<NodeFlagsType> >& node, const prev::common::TagSet& tagsToCheck, const LogicOperation operation) const
    {
        if (operation == LogicOperation::AND) {
            return node->GetTags().HasAll(tagsToCheck);
        } else if (operation == LogicOperation::OR) {
            return node->GetTags().HasAny(tagsToCheck);
        }
        return false;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindByIdInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const uint64_t id) const
    {
        if (parent->GetId() == id) {
            return parent;
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            auto result = FindByIdInternal(child, id);
            if (result != nullptr) {
                return result;
            }
        }
        return nullptr;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindOneWithFlagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const prev::common::FlagSet<NodeFlagsType>& flags, const LogicOperation operation) const
    {
        if (HasFlags(parent, flags, operation)) {
            return parent;
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            auto result = FindOneWithFlagsInternal(child, flags, operation);
            if (result != nullptr) {
                return result;
            }
        }

        return nullptr;
    }

    void FindAllWithFlagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const prev::common::FlagSet<NodeFlagsType>& flags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > >& result) const
    {
        if (HasFlags(parent, flags, operation)) {
            result.push_back(parent);
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            FindAllWithFlagsInternal(child, flags, operation, result);
        }
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindOneWithTagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const prev::common::TagSet& tags, const LogicOperation operation) const
    {
        if (HasTags(parent, tags, operation)) {
            return parent;
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            auto result = FindOneWithTagsInternal(child, tags, operation);
            if (result != nullptr) {
                return result;
            }
        }

        return nullptr;
    }

    void FindAllWithTagsInternal(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent, const prev::common::TagSet& tags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > >& result) const
    {
        if (HasTags(parent, tags, operation)) {
            result.push_back(parent);
        }

        auto& children = parent->GetChildren();
        for (auto& child : children) {
            FindAllWithTagsInternal(child, tags, operation, result);
        }
    }

public:
    void SetRootNode(const std::shared_ptr<ISceneNode<NodeFlagsType> >& root)
    {
        m_root = root;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetRootNode() const
    {
        return m_root;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindById(const uint64_t id) const
    {
        return FindByIdInternal(m_root, id);
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindOneWithFlags(const prev::common::FlagSet<NodeFlagsType>& flags, const LogicOperation operation = LogicOperation::OR) const
    {
        return FindOneWithFlagsInternal(m_root, flags, operation);
    }

    std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > FindAllWithFlags(const prev::common::FlagSet<NodeFlagsType>& flags, const LogicOperation operation = LogicOperation::OR) const
    {
        std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > result;
        FindAllWithFlagsInternal(m_root, flags, operation, result);
        return result;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > FindOneWithTags(const prev::common::TagSet& tags, const LogicOperation operation = LogicOperation::OR) const
    {
        return FindOneWithTagsInternal(m_root, tags, operation);
    }

    std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > FindAllWithTags(const prev::common::TagSet& tags, const LogicOperation operation = LogicOperation::OR) const
    {
        std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > result;
        FindAllWithTagsInternal(m_root, tags, operation, result);
        return result;
    }

private:
    GraphTraversal() = default;

private:
    friend class prev::common::pattern::Singleton<GraphTraversal<NodeFlagsType> >;

private:
    std::shared_ptr<ISceneNode<NodeFlagsType> > m_root;
};
} // namespace prev::scene::graph

#endif