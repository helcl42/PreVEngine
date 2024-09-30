#ifndef __CORE_H__
#define __CORE_H__

#include "../common/Common.h"

#ifdef ENABLE_VK_LOADER
#include <external/volk/volk.h>
#else
#include <vulkan/vulkan.h>
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
        if (VKRESULT) {                            \
            printf("%s:%d\n", __FILE__, __LINE__); \
        }                                          \
    }
#endif

#endif // !__CORE_H__
