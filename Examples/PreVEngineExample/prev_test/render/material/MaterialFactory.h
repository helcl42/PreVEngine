#ifndef __MATERIAL_FACTORY_H__
#define __MATERIAL_FACTORY_H__

#include "../IMaterial.h"

#include <prev/core/device/Device.h>

#include <memory>
#include <vector>

namespace prev_test::render::material {
class MaterialFactory final {
public:
    MaterialFactory(prev::core::device::Device& device);

    ~MaterialFactory() = default;

public:
    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps) const;

    // When async is true, the color texture is uploaded asynchronously (see ImageBufferBuilder::BuildAsync):
    // the material's image buffer is not ready until a later frame, and renderers must skip it until then.
    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath, bool async = false) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, bool async = false) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, const std::string& heightMapPath, bool async = false) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateCubeMap(const MaterialProperties& materialProps, const std::vector<std::string>& sidePaths, bool async = false) const;

    std::vector<std::shared_ptr<prev_test::render::IMaterial>> Create(const std::string& modelPath, bool async = false) const;

private:
    prev::core::device::Device& m_device;
};
} // namespace prev_test::render::material

#endif // !__MATERIAL_FACTORY_H__
