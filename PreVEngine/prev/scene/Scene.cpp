#include "Scene.h"

namespace prev::scene {
Scene::Scene(const std::shared_ptr<prev::scene::graph::ISceneNode>& rootNode)
    : m_rootNode{ rootNode }
{
}

void Scene::Init()
{
    m_rootNode->Init();
}

void Scene::Update(float deltaTime)
{
    m_rootNode->Update(deltaTime);
}

void Scene::ShutDown()
{
    m_rootNode->ShutDown();
    m_rootNode = nullptr;
}

std::shared_ptr<prev::scene::graph::ISceneNode> Scene::GetRootNode() const
{
    return m_rootNode;
}
} // namespace prev::scene