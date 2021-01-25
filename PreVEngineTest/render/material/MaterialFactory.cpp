#include "MaterialFactory.h"
#include "Material.h"

#include "../util/assimp/AssimpSceneLoader.h"

#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>

#include <prev/scene/AllocatorProvider.h>

#include <map>

namespace prev_test::render::util::assimp {
class AssimpMaterialFactory final {
public:
    bool CreateTexture(const aiScene& scene, const aiMaterial& material, const aiTextureType textureType, const bool filtering, const bool repeatAddressMode, prev::core::memory::Allocator& allocator, ImagePair& outImage) const
    {
        aiString textureFilePath;
        if (material.Get(AI_MATKEY_TEXTURE(textureType, 0), textureFilePath) != aiReturn_SUCCESS) {
            return false;
        }

        if (auto texture = scene.GetEmbeddedTexture(textureFilePath.C_Str())) {
            const auto image{ CreateImage(*texture) };
            const auto imageBuffer{ CreateImageBuffer(allocator, image, filtering, repeatAddressMode) };
            outImage = { image, imageBuffer };
        } else {
            const auto image{ CreateImage(textureFilePath.C_Str()) };
            const auto imageBuffer{ CreateImageBuffer(allocator, image, filtering, repeatAddressMode) };
            outImage = { image, imageBuffer };
        }
        return true;
    }

    MaterialProperties CreateMaterialProperties(const aiMaterial& material) const
    {
        aiColor3D color(1.0f, 1.0f, 1.0f);
        material.Get(AI_MATKEY_COLOR_DIFFUSE, color);

        float shineness;
        material.Get(AI_MATKEY_SHININESS, shineness);

        float reflectivity;
        material.Get(AI_MATKEY_REFLECTIVITY, reflectivity);

        MaterialProperties materialProperties{ { color.r, color.g, color.b, 1.0 }, shineness, std::max(reflectivity, 1.0f) };
        return materialProperties;
    }

private:
    std::shared_ptr<prev::render::image::Image> CreateImage(const std::string& textureFilename) const
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

    std::shared_ptr<prev::render::image::Image> CreateImage(const aiTexture& texture) const
    {
        prev::render::image::ImageFactory imageFactory;
        std::shared_ptr<prev::render::image::Image> image = imageFactory.CreateImageFromMemory(reinterpret_cast<uint8_t*>(texture.pcData), texture.mWidth);
        return image;
    }

    std::shared_ptr<prev::core::memory::image::IImageBuffer> CreateImageBuffer(prev::core::memory::Allocator& allocator, const std::shared_ptr<prev::render::image::Image>& image, const bool filtering, const bool repeatAddressMode) const
    {
        const VkExtent2D imageExtent = { image->GetWidth(), image->GetHeight() };

        auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
        imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ imageExtent, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, filtering, VK_IMAGE_VIEW_TYPE_2D, 1, repeatAddressMode ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });

        return imageBuffer;
    }

private:
    static inline std::map<std::string, std::shared_ptr<prev::render::image::Image> > s_imagesCache;
};
} // namespace prev_test::render::util::assimp

namespace prev_test::render::material {
std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const ImagePair& image) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps, image);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps, image, normalMap);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap, const ImagePair& heightMap) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps, image, normalMap, heightMap);
}

std::vector<std::unique_ptr<prev_test::render::IMaterial> > MaterialFactory::Create(const std::string& modelPath) const
{
    std::vector<std::unique_ptr<prev_test::render::IMaterial> > result;

    Assimp::Importer importer{};
    const aiScene* scene;

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, &importer, &scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev_test::render::util::assimp::AssimpMaterialFactory assimpMaterialFactory{};
    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
        const auto& material{ *scene->mMaterials[i] };

        ImagePair colorImage;
        assimpMaterialFactory.CreateTexture(*scene, material, aiTextureType_DIFFUSE, true, false, *allocator, colorImage);

        ImagePair normalImage;
        assimpMaterialFactory.CreateTexture(*scene, material, aiTextureType_NORMALS, true, false, *allocator, normalImage);

        ImagePair heightImage;
        assimpMaterialFactory.CreateTexture(*scene, material, aiTextureType_HEIGHT, false, false, *allocator, heightImage);

        const auto materialProperties{ assimpMaterialFactory.CreateMaterialProperties(material) };

        result.emplace_back(std::make_unique<prev_test::render::material::Material>(materialProperties, colorImage, normalImage, heightImage));
    }

    return result;
}
} // namespace prev_test::render::material