#ifndef __INSTANCE_FACTORY_H__
#define __INSTANCE_FACTORY_H__

#include "Instance.h"

#include "../engine/Config.h"

#include <memory>
#include <string>

namespace prev::core::instance {
class InstanceFactory final {
public:
    std::unique_ptr<Instance> Create(const std::string& appName, bool enableValidation, prev::core::engine::RenderBackend renderBackend) const;
};
} // namespace prev::core::instance

#endif