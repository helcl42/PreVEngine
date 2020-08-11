#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "Extensions.h"
#include "Layers.h"
#include "Validation.h"

namespace PreVEngine {
class Instance {
public:
    Instance(const Layers& layers, const Extensions& extensions, const char* appName = "PreVEngineApp", const char* engineName = "PreVEngine");

    Instance(const bool enableValidation = true, const char* appName = "PreVEngineApp", const char* engineName = "PreVEngine");

    ~Instance();

public:
    void Print() const;

    DebugReport& GetDebugReport();

    operator VkInstance() const;

private:
    void Create(const Layers& layers, const Extensions& extensions, const char* appName, const char* engineName);

private:
    VkInstance m_instance;

    DebugReport m_debugReport; // Configure debug report flags here.
};
} // namespace PreVEngine

#endif
