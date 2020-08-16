#include "Surface.h"
#include "../core/instance/Validation.h"

namespace prev::window {
Surface::Surface()
{
}

Surface::~Surface()
{
}

Surface::operator VkSurfaceKHR() const // Use *this as a VkSurfaceKHR
{
    return m_vkSurface;
}

bool Surface::CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const
{
    VkBool32 canPresent = false;
    VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queueFamily, m_vkSurface, &canPresent));
    return !!canPresent;
}
} // namespace prev::window