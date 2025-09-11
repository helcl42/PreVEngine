#ifndef __ISCENE_NODE_H__
#define __ISCENE_NODE_H__

#include "../component/ComponentRepository.h"

#include "../../common/Common.h"
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

    virtual void SetParent(const std::weak_ptr<ISceneNode>& parent) = 0;

    virtual std::shared_ptr<ISceneNode> GetParent() const = 0;

    virtual std::shared_ptr<ISceneNode> GetThis() const = 0;

    virtual std::shared_ptr<ISceneNode> GetRoot() const = 0;

    virtual bool IsRoot() const = 0;

    virtual uint64_t GetId() const = 0;

    virtual prev::common::TagSet& GetTags() = 0;

    virtual component::ComponentRepository& GetComponentRepository() = 0;

public:
    virtual ~ISceneNode() = default;
};
} // namespace prev::scene::graph

#endif // !__ISCENE_NODE_H__
