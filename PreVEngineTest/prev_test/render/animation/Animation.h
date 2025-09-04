#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "../IAnimation.h"

#include "AnimationClip.h"

#include <memory>

namespace prev_test::render::animation {
class Animation : public prev_test::render::IAnimation {
public:
    Animation(std::vector<std::unique_ptr<IAnimationClip>>&& clips);

    ~Animation() = default;

public:
    void Update(const float deltaTime) override;

    IAnimationClip& GetClip(const uint32_t clipIndex) const override;

    uint32_t GetClipCount() const override;

    void SetState(const AnimationState state) override;

    void SetSpeed(const float speed) override;

    void SetTime(const float elapsed) override;

private:
    std::vector<std::unique_ptr<IAnimationClip>> m_clips;
};

} // namespace prev_test::render::animation

#endif // !__ANIMATION_H__
