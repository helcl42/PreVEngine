#ifndef __PREV_TIME_REAL_TIME_PROVIDER_H__
#define __PREV_TIME_REAL_TIME_PROVIDER_H__

#include "ITimeProvider.h"

#include <chrono>

namespace prev::time {
class RealTimeProvider final : public ITimeProvider {
public:
    void Update() override;

    float GetDelta() const override;

private:
    std::chrono::steady_clock::time_point m_last{ std::chrono::steady_clock::now() };

    float m_delta{ 0.0f };
};
} // namespace prev::time

#endif // !__PREV_TIME_REAL_TIME_PROVIDER_H__
