#ifndef __IANIMATION_RENDER_COMPONENT_H__
#define __IANIMATION_RENDER_COMPONENT_H__

#include "IRenderComponent.h"

#include "../../render/IAnimation.h"

namespace prev_test::component::render {
class IAnimationRenderComponent : public IRenderComponent {
public:
    virtual std::shared_ptr<prev_test::render::IAnimation> GetAnimation() const = 0;

public:
    virtual ~IAnimationRenderComponent() = default;
};
} // namespace prev_test::component::render

#endif