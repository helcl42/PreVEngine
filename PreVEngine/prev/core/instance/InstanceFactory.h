#ifndef __INSTANCE_FACTORY_H__
#define __INSTANCE_FACTORY_H__

#include "Instance.h"

#include "../engine/Config.h"

#include <memory>
#include <string>
#include <vector>

namespace prev::core::instance {
class InstanceFactory final {
public:
    std::unique_ptr<Instance> Create(const std::string& appName, bool enableValidation, prev::core::engine::RenderBackend renderBackend, const std::vector<std::string>& nativeExtensions = {}) const;
};
} // namespace prev::core::instance

#endif