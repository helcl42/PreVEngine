#ifndef __MATERIAL_FACTORY_H__
#define __MATERIAL_FACTORY_H__

#include "../IMaterial.h"

#include <prev/common/Cache.h>
#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/image/Image.h>

#include <memory>
#include <vector>

namespace prev_test::render::material {
class MaterialFactory final {
public:
    MaterialFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~MaterialFactory() = default;

public:
    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, const std::string& heightMapPath) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateCubeMap(const MaterialProperties& materialProps, const std::vector<std::string>& sidePaths) const;

    std::vector<std::shared_ptr<prev_test::render::IMaterial>> Create(const std::string& modelPath) const;

private:
    std::shared_ptr<prev::render::image::IImage> CreateImage(const std::string& textureFilename) const;

    std::shared_ptr<prev::render::buffer::ImageBuffer> CreateImageBuffer(const prev::render::image::IImage& image, const bool generateMipMaps) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

private:
    static inline prev::common::Cache<std::string, std::shared_ptr<prev::render::image::IImage>> s_imagesCache;
};
} // namespace prev_test::render::material

#endif // !__MATERIAL_FACTORY_H__
