#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "Validation.h"

#include <memory>

namespace prev::core::instance {
class Instance {
public:
    Instance(const VkInstance instance, std::unique_ptr<ValidationReporter>&& validationReporter);

    ~Instance();

public:
    operator VkInstance() const;

private:
    VkInstance m_instance{};

    std::unique_ptr<ValidationReporter> m_validationReporter{};
};
} // namespace prev::core::instance

#endif
