#ifndef __IANIMATION_H__
#define __IANIMATION_H__

#include <prev/common/Common.h>

#include <vector>

namespace prev_test::render {
enum class AnimationState {
    RUNNING = 0,
    PAUSED,
    STOPPED
};

class IAnimation {
public:
    virtual ~IAnimation() = default;

public:
    virtual void Update(const float deltaTime) = 0;

    virtual const std::vector<glm::mat4>& GetBoneTransforms() const = 0;

    virtual void SetState(const AnimationState animationState) = 0;

    virtual AnimationState GetState() const = 0;

    virtual void SetIndex(const unsigned int index) = 0;

    virtual unsigned int GetIndex() const = 0;

    virtual void SetSpeed(const float speed) = 0;

    virtual float GetSpeed() const = 0;

    virtual void SetTime(const float elapsed) = 0;

    virtual float GetTime() const = 0;
};
} // namespace prev_test::render

#endif // !__IANIMATION_H__
