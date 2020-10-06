#include "TimeComponent.h"

namespace prev_test::component::time {
void TimeComponent::Update(float deltaTime)
{
    m_currentDelta = deltaTime;
    m_elapsedTime += deltaTime;
}

float TimeComponent::GetCurrentDeltaTime() const
{
    return m_currentDelta;
}

float TimeComponent::GetElapsedTime() const
{
    return m_elapsedTime;
}
} // namespace prev_test::component::time