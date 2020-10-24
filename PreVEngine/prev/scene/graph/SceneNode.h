#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include "../../util/Utils.h"
#include "../../event/EventsChannel.h"
#include "../SceneEvents.h"

#include "ISceneNode.h"

namespace prev::scene::graph {
template <typename NodeFlagsType>
class SceneNode : public std::enable_shared_from_this<ISceneNode<NodeFlagsType> >, public ISceneNode<NodeFlagsType> {
public:
    SceneNode()
        : m_id(prev::util::IDGenerator::Instance().GenrateNewId())
    {
    }

    SceneNode(const prev::common::FlagSet<NodeFlagsType>& flags)
        : m_id(prev::util::IDGenerator::Instance().GenrateNewId())
        , m_flags(flags)
    {
    }

    SceneNode(const prev::common::TagSet& tags)
        : m_id(prev::util::IDGenerator::Instance().GenrateNewId())
        , m_tags(tags)
    {
    }

    virtual ~SceneNode() = default;

public:
    virtual void Init() override
    {
        for (auto& child : m_children) {
            child->Init();
        }
    }

    virtual void Update(float deltaTime) override
    {
        for (auto& child : m_children) {
            child->Update(deltaTime);
        }
    }

    virtual void ShutDown() override
    {
        for (auto& child : m_children) {
            child->ShutDown();
        }

        prev::event::EventChannel::Post(prev::scene::SceneNodeShutDownEvent{ GetId() });
        m_flags = prev::common::FlagSet<NodeFlagsType>();
    }

public:
    const std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > >& GetChildren() const override
    {
        return m_children;
    }

    void AddChild(const std::shared_ptr<ISceneNode<NodeFlagsType> >& child) override
    {
        child->SetParent(this->shared_from_this());

        m_children.emplace_back(child);
    }

    void RemoveChild(const std::shared_ptr<ISceneNode<NodeFlagsType> >& child) override
    {
        child->SetParent(nullptr);

        for (auto it = m_children.begin(); it != m_children.end(); ++it) {
            if ((*it)->GetId() == child->GetId()) {
                m_children.erase(it);
                break;
            }
        }
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetThis() override
    {
        return this->shared_from_this();
    }

    void SetParent(const std::shared_ptr<ISceneNode<NodeFlagsType> >& parent) override
    {
        m_parent = parent;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetParent() const override
    {
        return m_parent.lock();
    }

    bool IsRoot() const override
    {
        return !m_parent.lock();
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetRoot() override
    {
        auto parent = m_parent.lock();
        if (parent == nullptr) {
            return GetThis();
        }

        while (parent != nullptr) {
            auto tempParent = parent->GetParent();
            if (tempParent == nullptr) {
                break;
            }
            parent = tempParent;
        }

        return parent;
    }

    uint64_t GetId() const override
    {
        return m_id;
    }

    void SetFlags(const prev::common::FlagSet<NodeFlagsType>& flags) override
    {
        m_flags = flags;
    }

    const prev::common::FlagSet<NodeFlagsType>& GetFlags() const override
    {
        return m_flags;
    }

    void SetTags(const prev::common::TagSet& tagSet) override
    {
        m_tags = tagSet;
    }

    const prev::common::TagSet& GetTags() const override
    {
        return m_tags;
    }

protected:
    uint64_t m_id;

    prev::common::FlagSet<NodeFlagsType> m_flags;

    prev::common::TagSet m_tags;

    std::weak_ptr<ISceneNode<NodeFlagsType> > m_parent;

    std::vector<std::shared_ptr<ISceneNode<NodeFlagsType> > > m_children;
};
} // namespace prev::scene::graph

#endif