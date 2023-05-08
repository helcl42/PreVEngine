#ifndef __MATERIAL_FACTORY_H__
#define __MATERIAL_FACTORY_H__

#include "../IMaterial.h"

#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/image/IImageBuffer.h>
#include <prev/render/image/Image.h>

#include <map>

namespace prev_test::render::material {
class MaterialFactory final {
public:
    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath, prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, const std::string& heightMapPath, prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateCubeMap(const MaterialProperties& materialProps, const std::vector<std::string>& sidePaths, prev::core::memory::Allocator& allocator) const;

    std::vector<std::shared_ptr<prev_test::render::IMaterial>> Create(const std::string& modelPath, prev::core::memory::Allocator& allocator) const;

private:
    std::shared_ptr<prev::render::image::Image> CreateImage(const std::string& textureFilename) const;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> CreateImageBuffer(const std::shared_ptr<prev::render::image::Image>& image, prev::core::memory::Allocator& allocator) const;

private:
    static inline std::map<std::string, std::shared_ptr<prev::render::image::Image>> s_imagesCache;
};
} // namespace prev_test::render::material

#endif // !__MATERIAL_FACTORY_H__
