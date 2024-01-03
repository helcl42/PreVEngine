#ifndef __RAY_CASTER_COMPONENT_FACTORY_H__
#define __RAY_CASTER_COMPONENT_FACTORY_H__

#include "IMouseRayCasterComponent.h"
#include "IRayCasterComponent.h"

namespace prev_test::component::ray_casting {
class RayCasterComponentFactory final {
public:
    std::unique_ptr<IRayCasterComponent> CreateRayCaster() const;

    std::unique_ptr<IMouseRayCasterComponent> CreateMouseRayCaster() const;
};
} // namespace prev_test::component::ray_casting

#endif // ! __RAY_CASTER_COMPONENT_FACTORY_H__
