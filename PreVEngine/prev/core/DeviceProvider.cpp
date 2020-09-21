#include "DeviceProvider.h"

#include <stdexcept>

namespace prev::core {
std::shared_ptr<prev::core::device::Device> DeviceProvider::GetDevice() const
{
    return m_device;
}

void DeviceProvider::SetDevice(const std::shared_ptr<prev::core::device::Device>& device)
{
    m_device = device;
}
} // namespace prev::core