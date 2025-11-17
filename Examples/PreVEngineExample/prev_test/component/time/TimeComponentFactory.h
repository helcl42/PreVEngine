#ifndef __TIME_COMPONENT_FACTORY_H__
#define __TIME_COMPONENT_FACTORY_H__

#include "ITimeComponent.h"

#include <memory>

namespace prev_test::component::time {
class TimeComponentFactory final {
public:
    std::unique_ptr<ITimeComponent> Create() const;
};
} // namespace prev_test::component::time

#endif // !__TIME_COMPONENT_FACTORY_H__
