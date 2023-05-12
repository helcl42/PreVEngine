#ifndef __SHADOWS_COMMON_H__
#define __SHADOWS_COMMON_H__

#include <prev/common/Common.h>
#include <prev/core/Formats.h>

namespace prev_test::component::shadow {

constexpr const bool SHADOWS_ENABLED{ true };

constexpr const uint32_t CASCADES_COUNT{ 4 };

constexpr const VkFormat DEPTH_FORMAT{ VK_FORMAT_D32_SFLOAT };

constexpr const uint32_t SHADOW_MAP_DIMENSIONS{ 2048 };

} // namespace prev_test::component::shadow

#endif // !__SHADOWS_COMMON_H__
