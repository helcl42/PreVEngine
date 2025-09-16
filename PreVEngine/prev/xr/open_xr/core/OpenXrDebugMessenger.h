#ifndef __OPEN_XR_DEBUG_MESSENGER_H__
#define __OPEN_XR_DEBUG_MESSENGER_H__

#ifdef ENABLE_XR

#include "../common/OpenXrCommon.h"

namespace prev::xr::open_xr::core {
class OpenXrDebugMessenger final {
public:
    explicit OpenXrDebugMessenger(XrInstance instance);

    ~OpenXrDebugMessenger();

private:
    XrDebugUtilsMessengerEXT m_debugUtilsMessenger{};
};
} // namespace prev::xr::open_xr::core

#endif

#endif