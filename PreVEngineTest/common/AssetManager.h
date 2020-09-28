#ifndef __ASSET_MANAGER_H__
#define __ASSET_MANAGER_H__

#include <prev/common/pattern/Singleton.h>

#include <string>

namespace prev_test::common {
class AssetManager : public prev::common::pattern::Singleton<AssetManager> {
public:
    ~AssetManager() = default;

public:
    std::string GetAssetPath(const std::string& path) const;

private:
    friend class prev::common::pattern::Singleton<AssetManager>;

private:
    AssetManager() = default;
};

} // namespace prev_test::common

#endif