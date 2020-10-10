#include "ShadowsComponentFactory.h"
#include "ShadowsCommon.h"
#include "ShadowsComponent.h"

namespace prev_test::component::shadow {
std::unique_ptr<IShadowsComponent> ShadowsComponentFactory::Create() const
{
    return std::make_unique<ShadowsComponent>(prev_test::component::shadow::CASCADES_COUNT);
}
} // namespace prev_test::component::shadow