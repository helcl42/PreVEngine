#include "AssetManager.h"

#include <prev/util/Utils.h>

#ifdef TARGET_PLATFORM_IOS
#include <IOSUtils.h>
#endif

namespace prev_test::common {
std::string AssetManager::GetAssetPath(const std::string& path) const
{
#if defined(TARGET_PLATFORM_ANDROID)
    return path;
#elif defined(TARGET_PLATFORM_IOS)
    return GetResourcesRootPath() + "/" + prev::util::file::GetFileName(path);
#else
    return "./assets/" + path;
#endif
}

} // namespace prev_test::common
