#ifndef __SKY_H__
#define __SKY_H__

#include "../../General.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::sky {
class Sky final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    Sky();

    ~Sky() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;
};
} // namespace prev_test::scene::sky

#endif // !__SKY_H__
