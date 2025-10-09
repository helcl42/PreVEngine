#ifndef __OPENXR_INPUT_UTILS_H__
#define __OPENXR_INPUT_UTILS_H__

#ifdef ENABLE_XR

#include "../../common/OpenXrCommon.h"

#include "../../../XrEvents.h"

#include "../../../../util/MathUtils.h"

#include <optional>
#include <vector>

namespace prev::xr::open_xr::input::util {
XrPath ConvertStringToXrPath(const XrInstance& instance, const char* pathString);

std::string ConvertXrPathToString(const XrInstance& instance, const XrPath& path);

XrAction CreateAction(const XrActionSet& actionSet, const char* name, const XrActionType xrActionType, const std::vector<XrPath>& subactionPaths);

bool SuggestProfileBindings(const XrInstance& instance, const char* profilePath, const std::vector<XrActionSuggestedBinding>& bindings);

XrSpace CreateActionSpace(const XrSession& session, const XrAction& xrAction, const XrPath& subactionPath);

void DestroyActionSpace(XrSpace& space);

prev::util::math::Pose ConvertXrPoseToPose(const XrPosef& xrPose);

XrPosef ConvertPoseToXrPose(const prev::util::math::Pose& pose);

std::optional<prev::util::math::Pose> GetPoseState(const XrSession& session, const XrTime& time, const XrAction& action, const XrPath& subActionPath, const XrSpace& baseSpace, const XrSpace& space);

std::optional<float> GetFloatState(const XrSession& session, const XrAction& action, const XrPath& subActionPath);

std::optional<bool> GetBoolState(const XrSession& session, const XrAction& action, const bool invert, const XrPath& subActionPath);

template <typename IndexType>
IndexType ConvertHandTypeToIndex(const HandType type)
{
    return static_cast<IndexType>(type);
}

template <typename IndexType>
HandType ConvertIndexToHandType(const IndexType index)
{
    return static_cast<HandType>(index);
}
} // namespace prev::xr::open_xr::input::util

#endif

#endif