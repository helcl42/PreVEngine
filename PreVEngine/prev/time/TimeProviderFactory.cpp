#include "TimeProviderFactory.h"

#include "FixedTimeProvider.h"
#include "RealTimeProvider.h"

namespace prev::time {
std::unique_ptr<ITimeProvider> TimeProviderFactory::Create(float fixedDelta) const
{
    if (fixedDelta > 0.0f) {
        return std::make_unique<FixedTimeProvider>(fixedDelta);
    }
    return std::make_unique<RealTimeProvider>();
}
} // namespace prev::time
