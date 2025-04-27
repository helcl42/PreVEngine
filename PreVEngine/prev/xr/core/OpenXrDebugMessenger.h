#ifndef __OPEN_XR_DEBUG_MESSENGER_H__
#define __OPEN_XR_DEBUG_MESSENGER_H__

#ifdef ENABLE_XR

#include "../common/OpenXrCommon.h"

namespace prev::xr::core {
class OpenXrDebugMessenger final {
public:
    explicit OpenXrDebugMessenger(XrInstance instance);

    ~OpenXrDebugMessenger();

private:
    XrDebugUtilsMessengerEXT m_debugUtilsMessenger{};
};
}

#endif

#endif