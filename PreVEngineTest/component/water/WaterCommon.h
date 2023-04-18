#ifndef __WATER_COMMON_H__
#define __WATER_COMMON_H__

#include <prev/common/Common.h>

namespace prev_test::component::water {

constexpr float WATER_TILE_SIZE{ 20.0f };
#ifdef WIN32
constexpr float WATER_LEVEL{ -12.0f };
#else
constexpr float WATER_LEVEL{ -4.0f };
#endif // WIN32
constexpr float WATER_CLIP_PLANE_OFFSET{ 0.2f };
constexpr float WATER_WAVE_SPEED{ 0.03f };
constexpr uint32_t REFRACTION_EXTENT_DIVIDER{ 3 };
constexpr uint32_t REFLECTION_EXTENT_DIVIDER{ 4 };
constexpr glm::vec4 WATER_COLOR{ 0.0f, 0.3f, 0.5f, 1.0f };

} // namespace prev_test::component::water

#endif // !__WATER_COMMON_H__
