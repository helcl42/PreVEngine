#ifndef __CORE_H__
#define __CORE_H__

#include "../common/Common.h"

//=========================================== Vulkan Wrapper ===========================================
//  By default, all Vulkan functions call the loader trampoline-code, which then calls the ICD or layers.
//  Alternatively, vulkan_wrapper.h can be used to replace all Vulkan functions with a dispatch-table,
//  which skips the loader, and calls the ICD directly, thereby improving performance.
//  Android has no loader, and always uses vulkan_wrapper.h.
//  For more details, see /source/loader/LoaderAndLayerInterface.md in the VS or LVL repo.
//
//  WARNING: If you enable USE_VULKAN_WRAPPER, make sure vulkan.h is NEVER #included before vulkan_wrapper.h
//
// #define USE_VULKAN_WRAPPER

#ifdef USE_VULKAN_WRAPPER
#include <vulkan_wrapper.h> // Android: This must be included AFTER native.h
#else
#include <vulkan/vulkan.h> // PC: Build dispatch table, so we can skip loader trampoline-code
#endif

void ShowVkResult(const VkResult err);
const char* VkResultStr(const VkResult err);

#ifdef NDEBUG
#define VKERRCHECK(VKFN) \
    {                    \
        (void)VKFN;      \
    }
#else
#define VKERRCHECK(VKFN)                           \
    {                                              \
        VkResult VKRESULT = VKFN;                  \
        ShowVkResult(VKRESULT);                    \
        assert(VKRESULT >= 0);                     \
        if (VKRESULT)                              \
            printf("%s:%d\n", __FILE__, __LINE__); \
    }
#endif

#endif // !__CORE_H__
