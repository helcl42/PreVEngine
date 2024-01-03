#ifndef __TIME_COMPONENT_H__
#define __TIME_COMPONENT_H__

#include "ITimeComponent.h"

namespace prev_test::component::time {
class TimeComponent final : public ITimeComponent {
public:
    TimeComponent() = default;

    ~TimeComponent() = default;

public:
    void Update(float deltaTime) override;

    float GetCurrentDeltaTime() const override;

    float GetElapsedTime() const override;

private:
    float m_currentDelta{ 0.0f };

    float m_elapsedTime{ 0.0f };
};
} // namespace prev_test::component::time

#endif // !__TIME_COMPONENT_H__