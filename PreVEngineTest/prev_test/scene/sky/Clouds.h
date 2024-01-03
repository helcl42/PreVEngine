#ifndef __CLOUDS_H__
#define __CLOUDS_H__

#include "../../General.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::sky {
class Clouds final : public prev::scene::graph::SceneNode {
public:
    Clouds();

    ~Clouds() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;
};
} // namespace prev_test::scene::sky

#endif // !__CLOUDS_H__