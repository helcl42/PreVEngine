#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "Extensions.h"
#include "Layers.h"
#include "Validation.h"

namespace prev::core::instance {
class Instance {
public:
    Instance(const Layers& layers, const Extensions& extensions, const char* appName = "PreVEngineApp", const char* engineName = "PreVEngine");

    Instance(const bool enableValidation = true, const char* appName = "PreVEngineApp", const char* engineName = "PreVEngine");

    ~Instance();

public:
    void Print() const;

    operator VkInstance() const;

private:
    void Create(const Layers& layers, const Extensions& extensions, const char* appName, const char* engineName);

private:
    VkInstance m_instance{};

    ValidationReporter m_validationReporter{};
};
} // namespace prev::core::instance

#endif
