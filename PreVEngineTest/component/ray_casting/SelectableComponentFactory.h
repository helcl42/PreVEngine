#ifndef __SELECTABLE_COMPONENT_FACTORY_H__
#define __SELECTABLE_COMPONENT_FACTORY_H__

#include "ISelectableComponent.h"

namespace prev_test::component::ray_casting {
class SelectableComponentFacrory final {
public:
    std::unique_ptr<ISelectableComponent> Create() const;
};
} // namespace prev_test::component::ray_casting

#endif // !__SELECTABLE_COMPONENT_FACTORY_H__
