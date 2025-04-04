#ifndef __RENDERER_UTILS_H__
#define __RENDERER_UTILS_H__

#include "../../common/intersection/Frustum.h"

#include <prev/scene/graph/ISceneNode.h>

#include <memory>

namespace prev_test::render::renderer {

bool IsVisible(const prev_test::common::intersection::Frustum* frustums, const uint32_t frustumCount, const std::shared_ptr<prev::scene::graph::ISceneNode>& node);

bool IsSelected(const std::shared_ptr<prev::scene::graph::ISceneNode>& node);

} // namespace prev_test::render::renderer

#endif