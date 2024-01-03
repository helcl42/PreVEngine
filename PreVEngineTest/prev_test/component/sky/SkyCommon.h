#ifndef __SKY_COMMON_H__
#define __SKY_COMMON_H__

#include <prev/common/Common.h>

namespace prev_test::component::sky {

constexpr float SKY_BOX_SIZE{ 300.0f };
constexpr glm::vec4 FOG_COLOR{ 0.47f, 0.53f, 0.58f, 1.0f };
constexpr float FOG_DENSITY{ 0.004f };
constexpr float FOG_GRADIENT{ 12.0f };

} // namespace prev_test::component::sky

#endif // !__SKY_COMPONENT_H__
