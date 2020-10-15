#include "SelectableComponentFactory.h"
#include "SelectableComponent.h"

namespace prev_test::component::ray_casting {
std::unique_ptr<ISelectableComponent> SelectableComponentFacrory::Create() const
{
    return std::make_unique<SelectableComponent>();
}
} // namespace prev_test::component::ray_casting