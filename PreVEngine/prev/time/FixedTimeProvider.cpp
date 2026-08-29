#include "FixedTimeProvider.h"

namespace prev::time {
FixedTimeProvider::FixedTimeProvider(float delta)
    : m_delta{ delta }
{
}

void FixedTimeProvider::Update()
{
}

float FixedTimeProvider::GetDelta() const
{
    return m_delta;
}
} // namespace prev::time
