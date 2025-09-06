#ifndef __INSTANCE_FACTORY_H__
#define __INSTANCE_FACTORY_H__

#include "Instance.h"

#include <memory>
#include <string>
#include <vector>

namespace prev::core::instance {
class InstanceFactory final {
public:
    std::unique_ptr<Instance> Create(const bool enableValidation, const std::string& appName = "PreVEngineApp", const std::vector<std::string>& extLayers = {}, const std::vector<std::string>& extExtensions = {}) const;
};
} // namespace prev::core::instance

#endif