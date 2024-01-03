#include "TimeComponentFactory.h"
#include "TimeComponent.h"

namespace prev_test::component::time {
std::unique_ptr<ITimeComponent> TimeComponentFactory::Create() const
{
    return std::make_unique<TimeComponent>();
}
} // namespace prev_test::component::time