#ifndef __GRAPH_TRAVERSAL_H__
#define __GRAPH_TRAVERSAL_H__

#include "../../common/pattern/Singleton.h"
#include "ISceneNode.h"

namespace prev::scene::graph {
enum class LogicOperation {
    OR,
    AND
};

class GraphTraversal final : public prev::common::pattern::Singleton<GraphTraversal > {
public:
    ~GraphTraversal() = default;

private:
    bool HasTags(const std::shared_ptr<ISceneNode >& node, const prev::common::TagSet& tagsToCheck, const LogicOperation operation) const
    {
        if (operation == LogicOperation::AND) {
            return node->GetTags().HasAll(tagsToCheck);
        } else if (operation == LogicOperation::OR) {
            return node->GetTags().HasAny(tagsToCheck);
        }
        return false;
    }

    std::shared_ptr<ISceneNode > FindByIdInternal(const std::shared_ptr<ISceneNode >& parent, const uint64_t id) const
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

    std::shared_ptr<ISceneNode > FindOneWithTagsInternal(const std::shared_ptr<ISceneNode >& parent, const prev::common::TagSet& tags, const LogicOperation operation) const
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

    void FindAllWithTagsInternal(const std::shared_ptr<ISceneNode >& parent, const prev::common::TagSet& tags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode > >& result) const
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
    void SetRootNode(const std::shared_ptr<ISceneNode >& root)
    {
        m_root = root;
    }

    std::shared_ptr<ISceneNode > GetRootNode() const
    {
        return m_root;
    }

    std::shared_ptr<ISceneNode > FindById(const uint64_t id) const
    {
        return FindByIdInternal(m_root, id);
    }

    std::shared_ptr<ISceneNode > FindOneWithTags(const prev::common::TagSet& tags, const LogicOperation operation = LogicOperation::OR) const
    {
        return FindOneWithTagsInternal(m_root, tags, operation);
    }

    std::vector<std::shared_ptr<ISceneNode > > FindAllWithTags(const prev::common::TagSet& tags, const LogicOperation operation = LogicOperation::OR) const
    {
        std::vector<std::shared_ptr<ISceneNode > > result;
        FindAllWithTagsInternal(m_root, tags, operation, result);
        return result;
    }

private:
    GraphTraversal() = default;

private:
    friend class prev::common::pattern::Singleton<GraphTraversal >;

private:
    std::shared_ptr<ISceneNode > m_root;
};
} // namespace prev::scene::graph

#endif