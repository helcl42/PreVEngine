#ifndef __IANIMATION_H__
#define __IANIMATION_H__

#include <prev/common/Common.h>

#include <memory>
#include <vector>

namespace prev_test::render {
constexpr uint32_t MAX_BONES_COUNT{ 100 };

enum class AnimationState {
    RUNNING = 0,
    PAUSED,
    STOPPED
};

class IAnimationPart {
public:
    virtual ~IAnimationPart() = default;

public:
    virtual void Update(const float deltaTime) = 0;

    virtual const std::vector<glm::mat4>& GetBoneTransforms() const = 0;

    virtual void SetState(const AnimationState animationState) = 0;

    virtual void SetSpeed(const float speed) = 0;

    virtual void SetTime(const float elapsed) = 0;
};

class IAnimation {
public:
    virtual ~IAnimation() = default;

public:
    virtual void Update(const float deltaTime) = 0;

    virtual std::shared_ptr<IAnimationPart> GetAnimationPart(unsigned int partIndex) const = 0;

    virtual const std::vector<std::shared_ptr<IAnimationPart>>& GetAnimationParts() const = 0;

    virtual void SetState(const AnimationState animationState) = 0;

    virtual void SetSpeed(const float speed) = 0;

    virtual void SetTime(const float elapsed) = 0;
};
} // namespace prev_test::render

#endif // !__IANIMATION_H__
