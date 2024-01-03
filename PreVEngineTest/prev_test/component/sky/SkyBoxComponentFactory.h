#ifndef __SKY_BOX_COMPONENT_FACTORY_H__
#define __SKY_BOX_COMPONENT_FACTORY_H__

#include "ISkyBoxComponent.h"

namespace prev_test::component::sky {
class SkyBoxComponentFactory final {
public:
    std::unique_ptr<ISkyBoxComponent> Create() const;
};
} // namespace prev_test::component::sky

#endif // !__SKY_BOX_COMPONENT_FACTORY_H__
