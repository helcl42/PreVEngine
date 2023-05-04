#include "MaterialFactory.h"
#include "Material.h"

#include "../util/assimp/AssimpMaterialFactory.h"
#include "../util/assimp/AssimpSceneLoader.h"

#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>

#include <prev/core/memory/Allocator.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::material {
std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, prev::core::memory::Allocator& allocator) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(image, allocator) };
    auto imageSampler{ std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(imageBuffer->GetMipLevels()), materialProps.addressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, ImagePair{ imageBuffer, imageSampler });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, prev::core::memory::Allocator& allocator) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(image, allocator) };
    auto imageSampler{ std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(imageBuffer->GetMipLevels()), materialProps.addressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };

    auto normalImage{ CreateImage(normalMapPath) };
    auto normalImageBuffer{ CreateImageBuffer(normalImage, allocator) };
    auto normalImageSampler{ std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(normalImageBuffer->GetMipLevels()), materialProps.addressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, ImagePair{ imageBuffer, imageSampler }, ImagePair{ normalImageBuffer, normalImageSampler });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, const std::string& heightMapPath, prev::core::memory::Allocator& allocator) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(image, allocator) };
    auto imageSampler{ std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(imageBuffer->GetMipLevels()), materialProps.addressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };

    auto normalImage{ CreateImage(normalMapPath) };
    auto normalImageBuffer{ CreateImageBuffer(normalImage, allocator) };
    auto normalImageSampler{ std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(normalImageBuffer->GetMipLevels()), materialProps.addressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };

    auto heightImage{ CreateImage(heightMapPath) };
    auto heightImageBuffer{ CreateImageBuffer(heightImage, allocator) };
    auto heightImageSampler{ std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(heightImageBuffer->GetMipLevels()), materialProps.addressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, ImagePair{ imageBuffer, imageSampler }, ImagePair{ normalImageBuffer, normalImageSampler }, ImagePair{ heightImageBuffer, heightImageSampler });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::CreateCubeMap(const MaterialProperties& materialProps, const std::vector<std::string>& sidePaths, prev::core::memory::Allocator& allocator) const
{
    prev::render::image::ImageFactory imageFactory{};

    std::vector<std::shared_ptr<prev::render::image::Image>> images{};
    for (const auto& faceFilePath : sidePaths) {
        images.emplace_back(imageFactory.CreateImage(faceFilePath));
    }

    std::vector<const uint8_t*> layersData{};
    for (const auto& image : images) {
        layersData.emplace_back(reinterpret_cast<const uint8_t*>(image->GetBuffer()));
    }

    auto cubeMapImageBuffer{ std::make_shared<prev::core::memory::image::ImageBuffer>(allocator) };
    cubeMapImageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ images[0]->GetWidth(), images[0]->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, true, VK_IMAGE_VIEW_TYPE_CUBE, static_cast<uint32_t>(images.size()), layersData });
    auto cubeMapSampler{ std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(cubeMapImageBuffer->GetMipLevels()), materialProps.addressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, ImagePair{ cubeMapImageBuffer, cubeMapSampler });
}

std::vector<std::shared_ptr<prev_test::render::IMaterial>> MaterialFactory::Create(const std::string& modelPath, prev::core::memory::Allocator& allocator) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> result;

    Assimp::Importer importer{};
    const aiScene* scene;

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, &importer, &scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    const VkSamplerAddressMode DefaultAddressMode{ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };

    prev_test::render::util::assimp::AssimpMaterialFactory assimpMaterialFactory{};
    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
        const auto& material{ *scene->mMaterials[i] };

        ImagePair colorImage;
        if (auto image = assimpMaterialFactory.CreateModelImage(*scene, material, aiTextureType_DIFFUSE)) {
            colorImage.imageBuffer = CreateImageBuffer(image, allocator);
            colorImage.imageSampler = std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(colorImage.imageBuffer->GetMipLevels()), DefaultAddressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f);
        }

        ImagePair normalImage;
        if (auto image = assimpMaterialFactory.CreateModelImage(*scene, material, aiTextureType_NORMALS)) {
            normalImage.imageBuffer = CreateImageBuffer(image, allocator);
            normalImage.imageSampler = std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(normalImage.imageBuffer->GetMipLevels()), DefaultAddressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f);
        }

        ImagePair heightImage;
        if (auto image = assimpMaterialFactory.CreateModelImage(*scene, material, aiTextureType_HEIGHT)) {
            heightImage.imageBuffer = CreateImageBuffer(image, allocator);
            heightImage.imageSampler = std::make_shared<prev::render::sampler::Sampler>(allocator.GetDevice(), static_cast<float>(heightImage.imageBuffer->GetMipLevels()), DefaultAddressMode, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f);
        }

        aiColor3D color(1.0f, 1.0f, 1.0f);
        material.Get(AI_MATKEY_COLOR_DIFFUSE, color);

        float shineness{ 1.0f };
        material.Get(AI_MATKEY_SHININESS, shineness);

        float reflectivity{ 1.0f };
        material.Get(AI_MATKEY_REFLECTIVITY, reflectivity);

        MaterialProperties materialProperties{ { color.r, color.g, color.b, 1.0 }, shineness, std::max(reflectivity, 1.0f), DefaultAddressMode };

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

std::shared_ptr<prev::core::memory::image::IImageBuffer> MaterialFactory::CreateImageBuffer(const std::shared_ptr<prev::render::image::Image>& image, prev::core::memory::Allocator& allocator) const
{
    const VkExtent2D imageExtent{ image->GetWidth(), image->GetHeight() };

    auto imageBuffer{ std::make_unique<prev::core::memory::image::ImageBuffer>(allocator) };
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ imageExtent, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, reinterpret_cast<uint8_t*>(image->GetBuffer()) });

    return imageBuffer;
}

} // namespace prev_test::render::material