#ifndef __RENDERER_UTILS_H__
#define __RENDERER_UTILS_H__

#include <prev/scene/graph/ISceneNode.h>
#include <prev/util/intersection/Frustum.h>

#include <memory>

namespace prev_test::render::renderer {

bool IsVisible(const prev::util::intersection::Frustum* frustums, const uint32_t frustumCount, const std::shared_ptr<prev::scene::graph::ISceneNode>& node);

bool IsSelected(const std::shared_ptr<prev::scene::graph::ISceneNode>& node);

} // namespace prev_test::render::renderer

#endif