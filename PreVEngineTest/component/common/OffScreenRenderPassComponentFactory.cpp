#include "OffScreenRenderPassComponentFactory.h"

#include "OffScreenRenderPassComponent.h"

namespace prev_test::component::common {
std::unique_ptr<IOffScreenRenderPassComponent> OffScreenRenderPassComponentFactory::Create(const int width, const int height) const
{
    return std::make_unique<OffScreenRenderPassComponent>(width, height);
}
} // namespace prev_test::component::common