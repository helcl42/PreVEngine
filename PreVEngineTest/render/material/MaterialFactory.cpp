#include "MaterialFactory.h"
#include "Material.h"

#include "../util/assimp/AssimpMaterialFactory.h"
#include "../util/assimp/AssimpSceneLoader.h"

#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>

#include <prev/core/memory/Allocator.h>

namespace prev_test::render::material {
std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, prev::core::memory::Allocator& allocator) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(image, true, materialProps.repeatAddressMode, allocator) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, ImagePair{ image, std::move(imageBuffer) });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, prev::core::memory::Allocator& allocator) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(image, true, materialProps.repeatAddressMode, allocator) };

    auto normalImage{ CreateImage(normalMapPath) };
    auto normalImageBuffer{ CreateImageBuffer(normalImage, true, materialProps.repeatAddressMode, allocator) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, ImagePair{ image, std::move(imageBuffer) }, ImagePair{ normalImage, std::move(normalImageBuffer) });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, const std::string& heightMapPath, prev::core::memory::Allocator& allocator) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(image, true, materialProps.repeatAddressMode, allocator) };

    auto normalImage{ CreateImage(normalMapPath) };
    auto normalImageBuffer{ CreateImageBuffer(normalImage, true, materialProps.repeatAddressMode, allocator) };

    auto heightImage{ CreateImage(heightMapPath) };
    auto heightImageBuffer{ CreateImageBuffer(heightImage, true, materialProps.repeatAddressMode, allocator) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, ImagePair{ image, std::move(imageBuffer) }, ImagePair{ normalImage, std::move(normalImageBuffer) }, ImagePair{ heightImage, std::move(heightImageBuffer) });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::CreateCubeMap(const MaterialProperties& materialProps, const std::vector<std::string>& sidePaths, prev::core::memory::Allocator& allocator) const
{
    prev::render::image::ImageFactory imageFactory{};

    std::vector<std::shared_ptr<prev::render::image::Image> > images{};
    for (const auto& faceFilePath : sidePaths) {
        images.emplace_back(imageFactory.CreateImage(faceFilePath));
    }

    std::vector<const uint8_t*> layersData{};
    for (const auto& image : images) {
        layersData.emplace_back(reinterpret_cast<const uint8_t*>(image->GetBuffer()));
    }

    auto imageBuffer{ std::make_unique<prev::core::memory::image::ImageBuffer>(allocator) };
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ images[0]->GetWidth(), images[0]->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, true, true, VK_IMAGE_VIEW_TYPE_CUBE, static_cast<uint32_t>(images.size()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, layersData });

    return std::make_unique<prev_test::render::material::Material>(materialProps, ImagePair{ images[0], std::move(imageBuffer) });
}

std::vector<std::shared_ptr<prev_test::render::IMaterial> > MaterialFactory::Create(const std::string& modelPath, prev::core::memory::Allocator& allocator) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial> > result;

    Assimp::Importer importer{};
    const aiScene* scene;

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, &importer, &scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    prev_test::render::util::assimp::AssimpMaterialFactory assimpMaterialFactory{};
    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
        const auto& material{ *scene->mMaterials[i] };

        ImagePair colorImage;
        if (auto image = assimpMaterialFactory.CreateModelImage(*scene, material, aiTextureType_DIFFUSE)) {
            colorImage.image = image;
            colorImage.imageBuffer = CreateImageBuffer(image, true, false, allocator);
        }

        ImagePair normalImage;
        if (auto image = assimpMaterialFactory.CreateModelImage(*scene, material, aiTextureType_NORMALS)) {
            normalImage.image = image;
            normalImage.imageBuffer = CreateImageBuffer(image, true, false, allocator);
        }

        ImagePair heightImage;
        if (auto image = assimpMaterialFactory.CreateModelImage(*scene, material, aiTextureType_HEIGHT)) {
            normalImage.image = image;
            heightImage.imageBuffer = CreateImageBuffer(image, true, false, allocator);
        }

        aiColor3D color(1.0f, 1.0f, 1.0f);
        material.Get(AI_MATKEY_COLOR_DIFFUSE, color);

        float shineness;
        material.Get(AI_MATKEY_SHININESS, shineness);

        float reflectivity;
        material.Get(AI_MATKEY_REFLECTIVITY, reflectivity);

        MaterialProperties materialProperties{ { color.r, color.g, color.b, 1.0 }, shineness, std::max(reflectivity, 1.0f), false };

        result.emplace_back(std::make_unique<prev_test::render::material::Material>(materialProperties, colorImage, normalImage, heightImage));
    }

    return result;
}

std::shared_ptr<prev::render::image::Image> MaterialFactory::CreateImage(const std::string& textureFilename) const
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

std::shared_ptr<prev::core::memory::image::IImageBuffer> MaterialFactory::CreateImageBuffer(const std::shared_ptr<prev::render::image::Image>& image, const bool filtering, const bool repeatAddressMode, prev::core::memory::Allocator& allocator) const
{
    const VkExtent2D imageExtent{ image->GetWidth(), image->GetHeight() };

    auto imageBuffer{ std::make_unique<prev::core::memory::image::ImageBuffer>(allocator) };
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ imageExtent, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, filtering, VK_IMAGE_VIEW_TYPE_2D, 1, repeatAddressMode ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, reinterpret_cast<uint8_t*>(image->GetBuffer()) });

    return imageBuffer;
}

} // namespace prev_test::render::material