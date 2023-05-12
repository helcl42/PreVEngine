#ifndef __IANIMATION_RENDER_COMPONENT_H__
#define __IANIMATION_RENDER_COMPONENT_H__

#include "IRenderComponent.h"

#include "../../render/IAnimation.h"

namespace prev_test::component::render {
class IAnimationRenderComponent : public IRenderComponent {
public:
    virtual std::shared_ptr<prev_test::render::IAnimation> GetAnimation(const uint32_t index = 0) const = 0;

    virtual const std::vector<std::shared_ptr<prev_test::render::IAnimation>>& GetAnimations() const = 0;

    virtual std::shared_ptr<prev_test::render::IAnimation> GetCurrentAnimation() const = 0;

    virtual void SetCurrentAnimationIndex(const uint32_t index) = 0;

public:
    virtual ~IAnimationRenderComponent() = default;
};
} // namespace prev_test::component::render

#endif