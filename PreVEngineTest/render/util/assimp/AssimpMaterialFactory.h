#ifndef __ASSIMP_MATERIAL_FACTORY_H__
#define __ASSIMP_MATERIAL_FACTORY_H__

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <prev/render/image/Image.h>

#include <map>
#include <memory>

namespace prev_test::render::util::assimp {
class AssimpMaterialFactory final {
public:
    std::shared_ptr<prev::render::image::Image> CreateModelImage(const aiScene& scene, const aiMaterial& material, const aiTextureType textureType) const;

private:
    std::shared_ptr<prev::render::image::Image> CreateImage(const std::string& textureFilename) const;

    std::shared_ptr<prev::render::image::Image> CreateImage(const aiTexture& texture) const;

private:
    static inline std::map<std::string, std::shared_ptr<prev::render::image::Image> > s_imagesCache;
};
} // namespace prev_test::render::util::assimp

#endif // !__ASSIMP_MATERIAL_FACTORY_H__
