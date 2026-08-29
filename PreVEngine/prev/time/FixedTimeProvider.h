#ifndef __PREV_TIME_FIXED_TIME_PROVIDER_H__
#define __PREV_TIME_FIXED_TIME_PROVIDER_H__

#include "ITimeProvider.h"

namespace prev::time {
class FixedTimeProvider final : public ITimeProvider {
public:
    explicit FixedTimeProvider(float delta);

public:
    void Update() override;

    float GetDelta() const override;

private:
    float m_delta;
};
} // namespace prev::time

#endif // !__PREV_TIME_FIXED_TIME_PROVIDER_H__
