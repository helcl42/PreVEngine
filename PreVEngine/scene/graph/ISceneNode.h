#ifndef __ISCENE_NODE_H__
#define __ISCENE_NODE_H__

#include "../../common/FlagSet.h"
#include "../../common/TagSet.h"
#include "../RenderContext.h"

#include <memory>
#include <vector>

namespace PreVEngine {
template <typename NodeFlagsType>
class ISceneNode {
public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Render(RenderContext& renderContext) = 0;

    virtual const std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > >& GetChildren() const = 0;

    virtual void AddChild(const std::shared_ptr<ISceneNode<NodeFlagsType> >& child) = 0;

    virtual void RemoveChild(const std::shared_ptr<ISceneNode<NodeFlagsType> >& child) = 0;

    virtual void SetParent(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent) = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType> > GetParent() const = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType> > GetThis() = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType> > GetRoot() = 0;

    virtual bool IsRoot() const = 0;

    virtual uint64_t GetId() const = 0;

    virtual void SetFlags(const FlagSet<NodeFlagsType>& flags) = 0;

    virtual const FlagSet<NodeFlagsType>& GetFlags() const = 0;

    virtual void SetTags(const TagSet& tags) = 0;

    virtual const TagSet& GetTags() const = 0;

public:
    virtual ~ISceneNode() = default;
};
} // namespace PreVEngine

#endif // !__ISCENE_NODE_H__
