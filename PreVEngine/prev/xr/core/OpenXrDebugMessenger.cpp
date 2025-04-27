#include "OpenXrDebugMessenger.h"

#ifdef ENABLE_XR

#include <sstream>

namespace prev::xr::core {
namespace {
    PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT{};
    PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT{};

    void LoadXrExtensionFunctions(XrInstance xrInstance) {
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction *) &xrCreateDebugUtilsMessengerEXT), "Failed to get xrCreateDebugUtilsMessengerEXT.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction *) &xrDestroyDebugUtilsMessengerEXT), "Failed to get xrDestroyDebugUtilsMessengerEXT.");
    }

    template <typename T>
    inline bool BitwiseCheck(const T& value, const T& checkValue)
    {
        return ((value & checkValue) == checkValue);
    }

    XrBool32 OpenXrMessageCallback(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageType, const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        // Lambda to covert an XrDebugUtilsMessageSeverityFlagsEXT to std::string. Bitwise check to concatenate multiple severities to the output string.
        auto GetMessageSeverityString = [](XrDebugUtilsMessageSeverityFlagsEXT messageSeverity) -> std::string {
            bool separator{ false };

            std::string msgFlags;
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
                msgFlags += "VERBOSE";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "INFO";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "WARN";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "ERROR";
            }
            return msgFlags;
        };
        // Lambda to covert an XrDebugUtilsMessageTypeFlagsEXT to std::string. Bitwise check to concatenate multiple types to the output string.
        auto GetMessageTypeString = [](XrDebugUtilsMessageTypeFlagsEXT messageType) -> std::string {
            bool separator{ false };

            std::string msgFlags;
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)) {
                msgFlags += "GEN";
                separator = true;
            }
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "SPEC";
                separator = true;
            }
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "PERF";
            }
            return msgFlags;
        };

        // Collect message data.
        std::string functionName = (pCallbackData->functionName) ? pCallbackData->functionName : "";
        std::string messageSeverityStr = GetMessageSeverityString(messageSeverity);
        std::string messageTypeStr = GetMessageTypeString(messageType);
        std::string messageId = (pCallbackData->messageId) ? pCallbackData->messageId : "";
        std::string message = (pCallbackData->message) ? pCallbackData->message : "";

        // String stream final message.
        std::stringstream errorMessage;
        errorMessage << functionName << "(" << messageSeverityStr << " / " << messageTypeStr << "): msgNum: " << messageId << " - " << message;

        // Log and debug break.
        LOGE("%s", errorMessage.str().c_str());
        if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
            ASSERT(false, "Error...");
        }
        return XrBool32();
    }
}

OpenXrDebugMessenger::OpenXrDebugMessenger(XrInstance instance)
{
    LoadXrExtensionFunctions(instance);

    XrDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debugUtilsMessengerCI.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                              XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCI.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                         XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
    debugUtilsMessengerCI.userCallback = (PFN_xrDebugUtilsMessengerCallbackEXT)OpenXrMessageCallback;
    debugUtilsMessengerCI.userData = nullptr;

    OPENXR_CHECK(xrCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCI, &m_debugUtilsMessenger), "Failed to create DebugUtilsMessenger.");
}

OpenXrDebugMessenger::~OpenXrDebugMessenger()
{
    OPENXR_CHECK(xrDestroyDebugUtilsMessengerEXT(m_debugUtilsMessenger), "Failed to destroy DebugUtilsMessenger.");
}
}

#endif // ENABLE_XR