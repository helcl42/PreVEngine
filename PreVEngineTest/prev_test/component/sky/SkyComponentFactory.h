#ifndef __SKY_COMPONENT_FACTORY_H__
#define __SKY_COMPONENT_FACTORY_H__

#include "ISkyComponent.h"

namespace prev_test::component::sky {
class SkyComponentFactory final {
public:
    std::unique_ptr<ISkyComponent> Create() const;
};
} // namespace prev_test::component::sky

#endif // !__SKY_COMPONENT_FACTORY_H__
