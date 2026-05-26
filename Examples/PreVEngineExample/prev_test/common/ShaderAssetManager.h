#ifndef __SHADER_ASSET_MANAGER_H__
#define __SHADER_ASSET_MANAGER_H__

#include <prev/common/pattern/Singleton.h>

#include <prev/core/Core.h>

#include <string>

namespace prev_test::common {

class ShaderAssetManager : public prev::common::pattern::Singleton<ShaderAssetManager> {
public:
    std::string GetAssetPath(GfxBackend backend, const std::string& name) const;

private:
    friend class prev::common::pattern::Singleton<ShaderAssetManager>;
    ShaderAssetManager() = default;

    static const char* GetBackendShaderDir(GfxBackend backend);
    static const char* GetBackendShaderExtension(GfxBackend backend);
};

} // namespace prev_test::common

#endif // !__SHADER_ASSET_MANAGER_H__
