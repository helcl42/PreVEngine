#include "Instance.h"

#include "../../common/Logger.h"

namespace prev::core::instance {
Instance::Instance(const VkInstance instance, std::unique_ptr<ValidationReporter>&& validationReporter)
    : m_instance{ instance }
    , m_validationReporter{ std::move(validationReporter) }
{
}

Instance::~Instance()
{
    m_validationReporter = nullptr; // Must be called BEFORE vkDestroyInstance()

    vkDestroyInstance(m_instance, nullptr);
    LOGI("Vulkan Instance destroyed");
}

Instance::operator VkInstance() const
{
    return m_instance;
}
} // namespace prev::core::instance
