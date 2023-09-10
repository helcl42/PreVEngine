#ifndef __SCENE_H__
#define __SCENE_H__

#include "IScene.h"

namespace prev::scene {
class Scene : public IScene {
public:
    Scene(const std::shared_ptr<prev::scene::graph::ISceneNode>& rootNode);

    virtual ~Scene() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

    std::shared_ptr<prev::scene::graph::ISceneNode> GetRootNode() const override;

protected:
    std::shared_ptr<prev::scene::graph::ISceneNode> m_rootNode{};
};
} // namespace prev::scene

#endif // !__SCENE_H__