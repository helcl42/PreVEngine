#ifndef __RENDERER_UTILS_H__
#define __RENDERER_UTILS_H__

#include "../../common/intersection/Frustum.h"

namespace prev_test::render::renderer {

bool IsVisible(const prev_test::common::intersection::Frustum* frustums, const uint32_t frustumCount, const uint64_t nodeId);

bool IsSelected(const uint64_t nodeId);

} // namespace prev_test::render::renderer

#endif