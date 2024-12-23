#ifndef __ENGINE_IMPL_FACTORY_H__
#define __ENGINE_IMPL_FACTORY_H__

#include "EngineImpl.h"

#include <memory>

namespace prev::core::engine::impl {
class EngineImplFactory final {
public:
    std::unique_ptr<EngineImpl> Create(const Config& config) const;
};
} // namespace prev::core::engine::impl

#endif