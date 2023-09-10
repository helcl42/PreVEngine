#ifndef __ISCENE_H__
#define __ISCENE_H__

#include "../scene/graph/ISceneNode.h"

#include <memory>

namespace prev::scene {
class IScene {
public:
    virtual void Init() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<prev::scene::graph::ISceneNode> GetRootNode() const = 0;

public:
    virtual ~IScene() = default;
};
} // namespace prev::scene

#endif // __ISCENE_H__
