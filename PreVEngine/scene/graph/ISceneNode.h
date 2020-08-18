#ifndef __ISCENE_NODE_H__
#define __ISCENE_NODE_H__

#include "../../common/FlagSet.h"
#include "../../common/TagSet.h"
#include "../../render/RenderContext.h"

#include <memory>
#include <vector>

namespace prev::scene::graph {
template <typename NodeFlagsType>
class ISceneNode {
public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Render(prev::render::RenderContext& renderContext) = 0;

    virtual const std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > >& GetChildren() const = 0;

    virtual void AddChild(const std::shared_ptr<ISceneNode<NodeFlagsType> >& child) = 0;

    virtual void RemoveChild(const std::shared_ptr<ISceneNode<NodeFlagsType> >& child) = 0;

    virtual void SetParent(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent) = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType> > GetParent() const = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType> > GetThis() = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType> > GetRoot() = 0;

    virtual bool IsRoot() const = 0;

    virtual uint64_t GetId() const = 0;

    virtual void SetFlags(const prev::common::FlagSet<NodeFlagsType>& flags) = 0;

    virtual const prev::common::FlagSet<NodeFlagsType>& GetFlags() const = 0;

    virtual void SetTags(const prev::common::TagSet& tags) = 0;

    virtual const prev::common::TagSet& GetTags() const = 0;

public:
    virtual ~ISceneNode() = default;
};
} // namespace prev::scene::graph

#endif // !__ISCENE_NODE_H__
