#include "SceneNode.h"

#include "../../event/EventsChannel.h"
#include "../../util/Utils.h"
#include "../SceneEvents.h"

namespace prev::scene::graph {
SceneNode::SceneNode()
    : m_id(prev::util::IDGenerator::Instance().GenrateNewId())
{
}

SceneNode::SceneNode(const prev::common::TagSet& tags)
    : m_id(prev::util::IDGenerator::Instance().GenrateNewId())
    , m_tags(tags)
{
}

void SceneNode::Init()
{
    for (auto& child : m_children) {
        child->Init();
    }
}

void SceneNode::Update(float deltaTime)
{
    for (auto& child : m_children) {
        child->Update(deltaTime);
    }
}

void SceneNode::ShutDown()
{
    for (auto& child : m_children) {
        child->ShutDown();
    }

    prev::event::EventChannel::Post(prev::scene::SceneNodeShutDownEvent{ GetId() });
    m_tags = prev::common::TagSet();
}

const std::vector<std::shared_ptr<ISceneNode> >& SceneNode::GetChildren() const
{
    return m_children;
}

void SceneNode::AddChild(const std::shared_ptr<ISceneNode>& child)
{
    child->SetParent(this->shared_from_this());

    m_children.emplace_back(child);
}

void SceneNode::RemoveChild(const std::shared_ptr<ISceneNode>& child)
{
    child->SetParent(nullptr);

    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
        if ((*it)->GetId() == child->GetId()) {
            m_children.erase(it);
            break;
        }
    }
}

std::shared_ptr<ISceneNode> SceneNode::GetThis()
{
    return this->shared_from_this();
}

void SceneNode::SetParent(const std::shared_ptr<ISceneNode>& parent)
{
    m_parent = parent;
}

std::shared_ptr<ISceneNode> SceneNode::GetParent() const
{
    return m_parent.lock();
}

bool SceneNode::IsRoot() const
{
    return !m_parent.lock();
}

std::shared_ptr<ISceneNode> SceneNode::GetRoot()
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

uint64_t SceneNode::GetId() const
{
    return m_id;
}

void SceneNode::SetTags(const prev::common::TagSet& tagSet)
{
    m_tags = tagSet;
}

const prev::common::TagSet& SceneNode::GetTags() const
{
    return m_tags;
}
} // namespace prev::scene::graph