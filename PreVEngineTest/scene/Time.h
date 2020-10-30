#ifndef __TIME_H__
#define __TIME_H__

#include "../General.h"
#include "../component/time/ITimeComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Time final : public prev::scene::graph::SceneNode {
public:
    Time();

    ~Time() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    std::shared_ptr<prev_test::component::time::ITimeComponent> m_timeComponent;
};
} // namespace prev_test::scene

#endif // !__TIME_H__
