#include "GraphTraversal.h"

namespace prev::scene::graph {
bool GraphTraversal::HasTags(const std::shared_ptr<ISceneNode>& node, const prev::common::TagSet& tagsToCheck, const LogicOperation operation) const
{
    if (operation == LogicOperation::AND) {
        return node->GetTags().HasAll(tagsToCheck);
    } else if (operation == LogicOperation::OR) {
        return node->GetTags().HasAny(tagsToCheck);
    }
    return false;
}

std::shared_ptr<ISceneNode> GraphTraversal::FindByIdInternal(const std::shared_ptr<ISceneNode>& parent, const uint64_t id) const
{
    if (parent->GetId() == id) {
        return parent;
    }

    const auto& children = parent->GetChildren();
    for (const auto& child : children) {
        auto result = FindByIdInternal(child, id);
        if (result != nullptr) {
            return result;
        }
    }
    return nullptr;
}

std::shared_ptr<ISceneNode> GraphTraversal::FindOneWithTagsInternal(const std::shared_ptr<ISceneNode>& parent, const prev::common::TagSet& tags, const LogicOperation operation) const
{
    if (HasTags(parent, tags, operation)) {
        return parent;
    }

    const auto& children = parent->GetChildren();
    for (const auto& child : children) {
        auto result = FindOneWithTagsInternal(child, tags, operation);
        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

void GraphTraversal::FindAllWithTagsInternal(const std::shared_ptr<ISceneNode>& parent, const prev::common::TagSet& tags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode> >& result) const
{
    if (HasTags(parent, tags, operation)) {
        result.push_back(parent);
    }

    const auto& children = parent->GetChildren();
    for (const auto& child : children) {
        FindAllWithTagsInternal(child, tags, operation, result);
    }
}

void GraphTraversal::SetRootNode(const std::shared_ptr<ISceneNode>& root)
{
    m_root = root;
}

std::shared_ptr<ISceneNode> GraphTraversal::GetRootNode() const
{
    return m_root;
}

std::shared_ptr<ISceneNode> GraphTraversal::FindById(const uint64_t id) const
{
    return FindByIdInternal(m_root, id);
}

std::shared_ptr<ISceneNode> GraphTraversal::FindOneWithTags(const prev::common::TagSet& tags, const LogicOperation operation) const
{
    return FindOneWithTagsInternal(m_root, tags, operation);
}

std::vector<std::shared_ptr<ISceneNode> > GraphTraversal::FindAllWithTags(const prev::common::TagSet& tags, const LogicOperation operation) const
{
    std::vector<std::shared_ptr<ISceneNode> > result;
    FindAllWithTagsInternal(m_root, tags, operation, result);
    return result;
}
} // namespace prev::scene::graph