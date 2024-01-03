#include "AssimpMaterialFactory.h"

#include <prev/render/image/ImageFactory.h>

namespace prev_test::render::util::assimp {
std::shared_ptr<prev::render::image::Image> AssimpMaterialFactory::CreateModelImage(const aiScene& scene, const aiMaterial& material, const aiTextureType textureType) const
{
    aiString textureFilePath;
    if (material.Get(AI_MATKEY_TEXTURE(textureType, 0), textureFilePath) != aiReturn_SUCCESS) {
        return nullptr;
    }

    if (auto texture = scene.GetEmbeddedTexture(textureFilePath.C_Str())) {
        return CreateImage(*texture);
    } else {
        return CreateImage(textureFilePath.C_Str());
    }
}

std::shared_ptr<prev::render::image::Image> AssimpMaterialFactory::CreateImage(const std::string& textureFilename) const
{
    std::shared_ptr<prev::render::image::Image> image;
    if (s_imagesCache.find(textureFilename) != s_imagesCache.cend()) {
        image = s_imagesCache[textureFilename];
    } else {
        prev::render::image::ImageFactory imageFactory;
        image = imageFactory.CreateImage(textureFilename);
        s_imagesCache[textureFilename] = image;
    }
    return image;
}

std::shared_ptr<prev::render::image::Image> AssimpMaterialFactory::CreateImage(const aiTexture& texture) const
{
    prev::render::image::ImageFactory imageFactory;
    std::shared_ptr<prev::render::image::Image> image = imageFactory.CreateImageFromMemory(reinterpret_cast<uint8_t*>(texture.pcData), texture.mWidth);
    return image;
}
} // namespace prev_test::render::util::assimp