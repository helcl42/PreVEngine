#include "AssetManager.h"

namespace prev_test::common {
std::string AssetManager::GetAssetPath(const std::string& path) const
{
#if defined(__ANDROID__)
    return path;
#else
    return "./assets/" + path;
#endif
}

} // namespace prev_test::common