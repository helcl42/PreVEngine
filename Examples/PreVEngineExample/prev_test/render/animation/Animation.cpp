#include "Animation.h"

namespace prev_test::render::animation {
Animation::Animation(std::vector<std::unique_ptr<IAnimationClip>>&& clips)
    : m_clips{ std::move(clips) }
{
}

void Animation::Update(const float deltaTime)
{
    for (auto& clip : m_clips) {
        clip->Update(deltaTime);
    }
}

IAnimationClip& Animation::GetClip(const uint32_t clipIndex) const
{
    return *m_clips[clipIndex];
}

uint32_t Animation::GetClipCount() const
{
    return static_cast<uint32_t>(m_clips.size());
}

void Animation::SetState(const AnimationState state)
{
    for (auto& clip : m_clips) {
        clip->SetState(state);
    }
}

void Animation::SetSpeed(const float speed)
{
    for (auto& clip : m_clips) {
        clip->SetSpeed(speed);
    }
}

void Animation::SetTime(const float elapsed)
{
    for (auto& clip : m_clips) {
        clip->SetTime(elapsed);
    }
}
} // namespace prev_test::render::animation