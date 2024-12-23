#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "Extensions.h"
#include "Layers.h"
#include "Validation.h"

#include <memory>
#include <string>
#include <vector>

namespace prev::core::instance {
class Instance {
public:
    Instance(const bool enableValidation = true, const std::vector<std::string>& extLayers = {}, const std::vector<std::string>& extExtensions = {}, const char* appName = "PreVEngineApp", const char* engineName = "PreVEngine");

    Instance(const Layers& layers, const Extensions& extensions, const char* appName = "PreVEngineApp", const char* engineName = "PreVEngine");

    ~Instance();

public:
    void Print() const;

    operator VkInstance() const;

private:
    void Create(const Layers& layers, const Extensions& extensions, const char* appName, const char* engineName);

private:
    VkInstance m_instance{};

    std::unique_ptr<ValidationReporter> m_validationReporter{};
};
} // namespace prev::core::instance

#endif
