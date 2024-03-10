#ifndef __ISCENE_NODE_H__
#define __ISCENE_NODE_H__

#include "../../common/FlagSet.h"
#include "../../common/TagSet.h"

#include <memory>
#include <vector>

namespace prev::scene::graph {
class ISceneNode {
public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual const std::vector<std::shared_ptr<ISceneNode>>& GetChildren() const = 0;

    virtual void AddChild(const std::shared_ptr<ISceneNode>& child) = 0;

    virtual void RemoveChild(const std::shared_ptr<ISceneNode>& child) = 0;

    virtual void RemoveAllChildren() = 0;

    virtual void SetParent(const std::shared_ptr<ISceneNode>& parent) = 0;

    virtual std::shared_ptr<ISceneNode> GetParent() const = 0;

    virtual std::shared_ptr<ISceneNode> GetThis() = 0;

    virtual std::shared_ptr<ISceneNode> GetRoot() = 0;

    virtual bool IsRoot() const = 0;

    virtual uint64_t GetId() const = 0;

    virtual void SetTags(const prev::common::TagSet& tags) = 0;

    virtual const prev::common::TagSet& GetTags() const = 0;

public:
    virtual ~ISceneNode() = default;
};
} // namespace prev::scene::graph

#endif // !__ISCENE_NODE_H__
