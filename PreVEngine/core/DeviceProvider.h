#ifndef __DEVICE_PROVIDER_H__
#define __DEVICE_PROVIDER_H__

#include "../common/pattern/Singleton.h"
#include "device/Device.h"

#include <memory>

namespace prev::core {
class DeviceProvider final : public prev::common::pattern::Singleton<DeviceProvider> {
public:
    ~DeviceProvider() = default;

public:
    std::shared_ptr<prev::core::device::Device> GetDevice() const;

    void SetDevice(const std::shared_ptr<prev::core::device::Device>& device);

private:
    friend class prev::common::pattern::Singleton<DeviceProvider>;

private:
    DeviceProvider() = default;

private:
    std::shared_ptr<prev::core::device::Device> m_device;
};
} // namespace prev::core

#endif