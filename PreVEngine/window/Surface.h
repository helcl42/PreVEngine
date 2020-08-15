#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "../core/instance/Validation.h"

namespace prev {

class Surface { // Vulkan Surface
public:
    Surface();

    virtual ~Surface();

public:
    operator VkSurfaceKHR() const;

    bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const; // Checks if surface can present given queue type.

protected:
    VkInstance m_vkInstance = VK_NULL_HANDLE;

    VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
};
} // namespace prev

#endif