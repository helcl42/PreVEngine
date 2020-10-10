#ifndef __SHADOWS_COMPONENT_FACTORY_H__
#define __SHADOWS_COMPONENT_FACTORY_H__

#include "IShadowsComponent.h"

namespace prev_test::component::shadow {
class ShadowsComponentFactory final {
public:
    std::unique_ptr<IShadowsComponent> Create() const;
};
} // namespace prev_test::component::shadow

#endif