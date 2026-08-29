#ifndef __PREV_TIME_TIME_PROVIDER_FACTORY_H__
#define __PREV_TIME_TIME_PROVIDER_FACTORY_H__

#include "ITimeProvider.h"

#include <memory>

namespace prev::time {
class TimeProviderFactory final {
public:
    std::unique_ptr<ITimeProvider> Create(float fixedDelta = 0.0f) const;
};
} // namespace prev::time

#endif // !__PREV_TIME_TIME_PROVIDER_FACTORY_H__
