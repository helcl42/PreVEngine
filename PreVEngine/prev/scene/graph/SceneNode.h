#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include "ISceneNode.h"

namespace prev::scene::graph {
class SceneNode : public std::enable_shared_from_this<ISceneNode>, public ISceneNode {
public:
    SceneNode();

    SceneNode(const prev::common::TagSet& tags);

    virtual ~SceneNode() = default;

public:
    virtual void Init() override;

    virtual void Update(float deltaTime) override;

    virtual void ShutDown() override;

public:
    const std::vector<std::shared_ptr<ISceneNode>>& GetChildren() const override;

    void AddChild(const std::shared_ptr<ISceneNode>& child) override;

    void RemoveChild(const std::shared_ptr<ISceneNode>& child) override;

    void RemoveAllChildren() override;

    std::shared_ptr<ISceneNode> GetThis() const override;

    void SetParent(const std::weak_ptr<ISceneNode>& parent) override;

    std::shared_ptr<ISceneNode> GetParent() const override;

    bool IsRoot() const override;

    std::shared_ptr<ISceneNode> GetRoot() const override;

    uint64_t GetId() const override;

    prev::common::TagSet& GetTags() override;

    component::ComponentRepository& GetComponentRepository() override;

protected:
    uint64_t m_id;

    prev::common::TagSet m_tags;

    component::ComponentRepository m_componentsRepository;

    std::weak_ptr<ISceneNode> m_parent;

    std::vector<std::shared_ptr<ISceneNode>> m_children;
};
} // namespace prev::scene::graph

#endif