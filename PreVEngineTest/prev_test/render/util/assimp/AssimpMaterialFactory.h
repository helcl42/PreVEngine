#ifndef __ASSIMP_MATERIAL_FACTORY_H__
#define __ASSIMP_MATERIAL_FACTORY_H__

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <prev/common/Cache.h>
#include <prev/render/image/Image.h>

#include <memory>

namespace prev_test::render::util::assimp {
class AssimpMaterialFactory final {
public:
    explicit AssimpMaterialFactory(prev::common::Cache<std::string, std::shared_ptr<prev::render::image::IImage>>& imageCache);
    
    ~AssimpMaterialFactory() = default;

public:
    std::shared_ptr<prev::render::image::IImage> CreateModelImage(const aiScene& scene, const aiMaterial& material, const aiTextureType textureType) const;

private:
    std::shared_ptr<prev::render::image::IImage> CreateImage(const std::string& textureFilename) const;

    std::shared_ptr<prev::render::image::IImage> CreateImage(const aiTexture& texture) const;

private:
    prev::common::Cache<std::string, std::shared_ptr<prev::render::image::IImage>>& m_imageCache;
};
} // namespace prev_test::render::util::assimp

#endif // !__ASSIMP_MATERIAL_FACTORY_H__
