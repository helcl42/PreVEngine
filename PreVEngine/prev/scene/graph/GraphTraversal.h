#ifndef __GRAPH_TRAVERSAL_H__
#define __GRAPH_TRAVERSAL_H__

#include "ISceneNode.h"

namespace prev::scene::graph {
enum class LogicOperation {
    OR,
    AND
};

class GraphTraversal final {
public:
    static std::shared_ptr<ISceneNode> FindById(const std::shared_ptr<ISceneNode>& root, const uint64_t id);

    static std::shared_ptr<ISceneNode> FindByTags(const std::shared_ptr<ISceneNode>& root, const prev::common::TagSet& tags, const LogicOperation operation = LogicOperation::OR);

    static std::vector<std::shared_ptr<ISceneNode>> FindAllByTags(const std::shared_ptr<ISceneNode>& root, const prev::common::TagSet& tags, const LogicOperation operation = LogicOperation::OR);

private:
    static bool HasTags(const std::shared_ptr<ISceneNode>& node, const prev::common::TagSet& tagsToCheck, const LogicOperation operation);

    static std::shared_ptr<ISceneNode> FindByIdInternal(const std::shared_ptr<ISceneNode>& parent, const uint64_t id);

    static std::shared_ptr<ISceneNode> FindByTagsInternal(const std::shared_ptr<ISceneNode>& parent, const prev::common::TagSet& tags, const LogicOperation operation);

    static void FindAllByTagsInternal(const std::shared_ptr<ISceneNode>& parent, const prev::common::TagSet& tags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode>>& result);
};
} // namespace prev::scene::graph

#endif