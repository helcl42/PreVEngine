#include "RayCasterComponentFactory.h"
#include "MouseRayCasterComponent.h"
#include "RayCasterComponent.h"

namespace prev_test::component::ray_casting {
std::unique_ptr<IRayCasterComponent> RayCasterComponentFactory::CreateRayCaster() const
{
    return std::make_unique<RayCasterComponent>();
}

std::unique_ptr<IMouseRayCasterComponent> RayCasterComponentFactory::CreateMouseRayCaster() const
{
    return std::make_unique<MouseRayCasterComponent>();
}
} // namespace prev_test::component::ray_casting