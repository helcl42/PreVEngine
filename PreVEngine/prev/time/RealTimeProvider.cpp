#include "RealTimeProvider.h"

namespace prev::time {
void RealTimeProvider::Update()
{
    const auto now{ std::chrono::steady_clock::now() };
    m_delta = std::chrono::duration<float>(now - m_last).count();
    m_last = now;
}

float RealTimeProvider::GetDelta() const
{
    return m_delta;
}
} // namespace prev::time
