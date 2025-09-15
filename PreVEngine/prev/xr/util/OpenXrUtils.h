#ifndef __OPENXR_UTILS_H__
#define __OPENXR_UTILS_H__

#ifdef ENABLE_XR

#include "../common/OpenXrCommon.h"

namespace prev::xr::util {
template <typename T>
T CreateStruct(const XrStructureType structType, void* next = nullptr)
{
    T vkStruct = {}; // Uniform initialization to zero-out all members
    vkStruct.type = structType;
    vkStruct.next = next;
    return vkStruct;
}

} // namespace prev::xr::util

#endif

#endif