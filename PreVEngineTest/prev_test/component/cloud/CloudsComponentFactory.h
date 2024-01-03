#ifndef __CLOUDS_COMPONENT_FACTORY_H__
#define __CLOUDS_COMPONENT_FACTORY_H__

#include "ICloudsComponent.h"

namespace prev_test::component::cloud {
class CloudsComponentFactory final {
public:
    std::unique_ptr<ICloudsComponent> Create() const;
};
} // namespace prev_test::component::cloud

#endif // !__CLOUDS_COMPONENT_FACTORY_H__
