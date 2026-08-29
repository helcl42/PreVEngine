#ifndef __PREV_TIME_I_TIME_PROVIDER_H__
#define __PREV_TIME_I_TIME_PROVIDER_H__

namespace prev::time {
class ITimeProvider {
public:
    virtual ~ITimeProvider() = default;

public:
    virtual void Update() = 0;

    virtual float GetDelta() const = 0;
};
} // namespace prev::time

#endif // !__PREV_TIME_I_TIME_PROVIDER_H__
