#include "Surface.h"

#include "../../core/Core.h"

namespace prev::window::impl {
Surface::operator VkSurfaceKHR() const // Use *this as a VkSurfaceKHR
{
    return m_vkSurface;
}

bool Surface::CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const
{
    VkBool32 canPresent{ VK_FALSE };
    VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queueFamily, m_vkSurface, &canPresent));
    return !!canPresent;
}
} // namespace prev::window::impl