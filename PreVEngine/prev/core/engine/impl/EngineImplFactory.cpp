#include "EngineImplFactory.h"

#include "DefaultEngineImpl.h"
#if defined(__EMSCRIPTEN__)
#include "WebEngineImpl.h"
#endif
#ifdef ENABLE_XR
#include "XrEngineImpl.h"
#endif

namespace prev::core::engine::impl {
std::unique_ptr<EngineImpl> EngineImplFactory::Create(const Config& config) const
{
#ifdef ENABLE_XR
    return std::make_unique<XrEngineImpl>(config);
#elif defined(__EMSCRIPTEN__)
    return std::make_unique<WebEngineImpl>(config);
#else
    return std::make_unique<DefaultEngineImpl>(config);
#endif
}
} // namespace prev::core::engine::impl