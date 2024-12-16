#include "EngineImplFactory.h"

#include "DefaultEngineImpl.h"
#ifdef ENABLE_XR
#include "XrEngineImpl.h"
#endif

namespace prev::core::engine::impl {
std::unique_ptr<EngineImpl> EngineImplFactory::Create(const Config& config) const {
#ifdef ENABLE_XR
    return std::make_unique<XrEngineImpl>(config);
#else
    return std::make_unique<DefaultEngineImpl>(config);
#endif
}
}