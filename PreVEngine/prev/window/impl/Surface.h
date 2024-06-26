#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "../../core/Core.h"

namespace prev::window::impl {
class Surface {
public:
    Surface() = default;

    virtual ~Surface() = default;

public:
    operator VkSurfaceKHR() const;

    bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const; // Checks if surface can present given queue type.

protected:
    VkSurfaceKHR m_vkSurface{ VK_NULL_HANDLE };
};
} // namespace prev::window::impl

#endif