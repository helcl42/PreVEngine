#include "GraphTraversal.h"

namespace prev::scene::graph {
std::shared_ptr<ISceneNode> GraphTraversal::FindById(const std::shared_ptr<ISceneNode>& root, const uint64_t id)
{
    return FindByIdInternal(root, id);
}

std::shared_ptr<ISceneNode> GraphTraversal::FindByTags(const std::shared_ptr<ISceneNode>& root, const prev::common::TagSet& tags, const LogicOperation operation)
{
    return FindByTagsInternal(root, tags, operation);
}

std::vector<std::shared_ptr<ISceneNode>> GraphTraversal::FindAllByTags(const std::shared_ptr<ISceneNode>& root, const prev::common::TagSet& tags, const LogicOperation operation)
{
    std::vector<std::shared_ptr<ISceneNode>> result;
    FindAllByTagsInternal(root, tags, operation, result);
    return result;
}

bool GraphTraversal::HasTags(const std::shared_ptr<ISceneNode>& node, const prev::common::TagSet& tagsToCheck, const LogicOperation operation)
{
    if (operation == LogicOperation::AND) {
        return node->GetTags().HasAll(tagsToCheck);
    } else if (operation == LogicOperation::OR) {
        return node->GetTags().HasAny(tagsToCheck);
    }
    return false;
}

std::shared_ptr<ISceneNode> GraphTraversal::FindByIdInternal(const std::shared_ptr<ISceneNode>& parent, const uint64_t id)
{
    if (parent->GetId() == id) {
        return parent;
    }

    const auto& children{ parent->GetChildren() };
    for (const auto& child : children) {
        const auto result{ FindByIdInternal(child, id) };
        if (result) {
            return result;
        }
    }
    return nullptr;
}

std::shared_ptr<ISceneNode> GraphTraversal::FindByTagsInternal(const std::shared_ptr<ISceneNode>& parent, const prev::common::TagSet& tags, const LogicOperation operation)
{
    if (HasTags(parent, tags, operation)) {
        return parent;
    }

    const auto& children{ parent->GetChildren() };
    for (const auto& child : children) {
        const auto result{ FindByTagsInternal(child, tags, operation) };
        if (result) {
            return result;
        }
    }

    return nullptr;
}

void GraphTraversal::FindAllByTagsInternal(const std::shared_ptr<ISceneNode>& parent, const prev::common::TagSet& tags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode>>& result)
{
    if (HasTags(parent, tags, operation)) {
        result.push_back(parent);
    }

    const auto& children{ parent->GetChildren() };
    for (const auto& child : children) {
        FindAllByTagsInternal(child, tags, operation, result);
    }
}
} // namespace prev::scene::graph