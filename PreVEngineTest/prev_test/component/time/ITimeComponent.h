#ifndef __ITIME_COMPONENE_H__
#define __ITIME_COMPONENE_H__

namespace prev_test::component::time {
class ITimeComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual float GetCurrentDeltaTime() const = 0;

    virtual float GetElapsedTime() const = 0;

public:
    virtual ~ITimeComponent() = default;
};
} // namespace prev_test::component::time

#endif // !__ITIME_COMPONENE_H__
