#include "SceneNode.h"

#include "../../util/Utils.h"

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
    RemoveAllChildren();

    m_componentsRepository = {};
    m_tags = {};
}

const std::vector<std::shared_ptr<ISceneNode>>& SceneNode::GetChildren() const
{
    return m_children;
}

void SceneNode::AddChild(const std::shared_ptr<ISceneNode>& child)
{
    child->SetParent(GetThis());

    m_children.emplace_back(child);
}

void SceneNode::RemoveChild(const std::shared_ptr<ISceneNode>& child)
{
    child->SetParent({});

    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
        if ((*it)->GetId() == child->GetId()) {
            m_children.erase(it);
            break;
        }
    }
}

void SceneNode::RemoveAllChildren()
{
    for (auto& child : m_children) {
        child->SetParent({});
    }
    m_children.clear();
}

std::shared_ptr<ISceneNode> SceneNode::GetThis() const
{
    return const_cast<SceneNode*>(this)->shared_from_this();
}

void SceneNode::SetParent(const std::weak_ptr<ISceneNode>& parent)
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

std::shared_ptr<ISceneNode> SceneNode::GetRoot() const
{
    auto thizz{ GetThis() };
    auto parent{ thizz->GetParent() };
    while(parent) {
        thizz = parent;
        parent = parent->GetParent();
    }
    return thizz;
}

uint64_t SceneNode::GetId() const
{
    return m_id;
}

prev::common::TagSet& SceneNode::GetTags()
{
    return m_tags;
}

component::ComponentRepository& SceneNode::GetComponentRepository()
{
    return m_componentsRepository;
}
} // namespace prev::scene::graph