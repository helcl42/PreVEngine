#ifndef __SHADOW_H__
#define __SHADOW_H__

#include "../../General.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::shadow {
class Shadows final : public prev::scene::graph::SceneNode {
public:
    Shadows();

    virtual ~Shadows() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;
};
} // namespace prev_test::scene::shadow

#endif // !__SHADOW_H__
