#ifndef __GRAPH_TRAVERSAL_H__
#define __GRAPH_TRAVERSAL_H__

#include "../../common/pattern/Singleton.h"

#include "ISceneNode.h"

namespace prev::scene::graph {
enum class LogicOperation {
    OR,
    AND
};

class GraphTraversal final : public prev::common::pattern::Singleton<GraphTraversal> {
public:
    ~GraphTraversal() = default;

private:
    bool HasTags(const std::shared_ptr<ISceneNode>& node, const prev::common::TagSet& tagsToCheck, const LogicOperation operation) const;

    std::shared_ptr<ISceneNode> FindByIdInternal(const std::shared_ptr<ISceneNode>& parent, const uint64_t id) const;

    std::shared_ptr<ISceneNode> FindOneWithTagsInternal(const std::shared_ptr<ISceneNode>& parent, const prev::common::TagSet& tags, const LogicOperation operation) const;

    void FindAllWithTagsInternal(const std::shared_ptr<ISceneNode>& parent, const prev::common::TagSet& tags, const LogicOperation operation, std::vector<std::shared_ptr<ISceneNode> >& result) const;

public:
    void SetRootNode(const std::shared_ptr<ISceneNode>& root);

    std::shared_ptr<ISceneNode> GetRootNode() const;

    std::shared_ptr<ISceneNode> FindById(const uint64_t id) const;

    std::shared_ptr<ISceneNode> FindOneWithTags(const prev::common::TagSet& tags, const LogicOperation operation = LogicOperation::OR) const;

    std::vector<std::shared_ptr<ISceneNode> > FindAllWithTags(const prev::common::TagSet& tags, const LogicOperation operation = LogicOperation::OR) const;

private:
    GraphTraversal() = default;

private:
    friend class prev::common::pattern::Singleton<GraphTraversal>;

private:
    std::shared_ptr<ISceneNode> m_root;
};
} // namespace prev::scene::graph

#endif