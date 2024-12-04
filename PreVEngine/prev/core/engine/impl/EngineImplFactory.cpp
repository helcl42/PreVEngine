#include "EngineImplFactory.h"

#include "DefaultEngineImpl.h"
#include "XrEngineImpl.h"

namespace prev::core::engine::impl {
std::unique_ptr<EngineImpl> EngineImplFactory::Create(const Config& config) const {
#ifdef ENABLE_XR
    return std::make_unique<XrEngineImpl>(config);
#else
    return std::make_unique<DefaultEngineImpl>(config);
#endif
}
}