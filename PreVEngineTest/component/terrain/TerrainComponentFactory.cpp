#include "TerrainComponentFactory.h"
#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshUtil.h"
#include "../../render/model/ModelFactory.h"
#include "HeightGenerator.h"
#include "TerrainComponent.h"
#include "TerrainMesh.h"

#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::terrain {
TerrainComponentFactory::TerrainComponentFactory(const unsigned int seed, const unsigned int vertexCount)
    : m_seed(seed)
    , m_vertexCount(vertexCount)
{
}

std::unique_ptr<ITerrainComponenet> TerrainComponentFactory::CreateRandomTerrain(const int x, const int z, const float size) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();
    const auto heightGenerator = std::make_shared<HeightGenerator>(x, z, m_vertexCount, m_seed);

    const float layerTransitionWidth = 0.1f;
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), 10.0f, 0.2f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), 10.0f, 0.2f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), 10.0f, 0.2f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), 10.0f, 0.2f, 0.9f }
    };

    const std::shared_ptr<HeightMapInfo> heightMap = CreateHeightMap(heightGenerator);
    const std::shared_ptr<VertexData> vertexData = GenerateVertexData(heightMap, size);

    auto result = std::make_unique<TerrainComponent>(x, z);
    result->m_model = std::move(CreateModel(*allocator, vertexData, false));
    result->m_heightsInfo = heightMap;
    result->m_vertexData = vertexData;
    for (const auto& layer : terrainLayers) {
        auto material = CreateMaterial(*allocator, layer.materialPath, layer.shineDamper, layer.reflectivity);
        result->m_materials.emplace_back(std::move(material));
        result->m_heightSteps.emplace_back(layer.heightStep);
    }
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<ITerrainComponenet> TerrainComponentFactory::CreateRandomTerrainNormalMapped(const int x, const int z, const float size) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();
    const auto heightGenerator = std::make_shared<HeightGenerator>(x, z, m_vertexCount, m_seed);

    const float layerTransitionWidth = 0.1f;
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_normal.png"), 10.0f, 0.2f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal.png"), 10.0f, 0.2f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), 10.0f, 0.2f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_normal.png"), 10.0f, 0.2f, 0.9f }
    };

    const std::shared_ptr<HeightMapInfo> heightMap = CreateHeightMap(heightGenerator);
    const std::shared_ptr<VertexData> vertexData = GenerateVertexData(heightMap, size);

    auto result = std::make_unique<TerrainComponent>(x, z);
    result->m_model = std::move(CreateModel(*allocator, vertexData, true));
    result->m_heightsInfo = CreateHeightMap(heightGenerator);
    result->m_vertexData = vertexData;
    for (const auto& layer : terrainLayers) {
        auto material = CreateMaterial(*allocator, layer.materialPath, layer.materialNormalPath, layer.shineDamper, layer.reflectivity);
        result->m_materials.emplace_back(std::move(material));
        result->m_heightSteps.emplace_back(layer.heightStep);
    }
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<ITerrainComponenet> TerrainComponentFactory::CreateRandomTerrainParallaxMapped(const int x, const int z, const float size) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();
    const auto heightGenerator = std::make_shared<HeightGenerator>(x, z, m_vertexCount, m_seed);

    const float layerTransitionWidth = 0.1f;
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_normal_2.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_height.png"), 10.0f, 0.2f, 0.000002f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal_2.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_height.png"), 10.0f, 0.2f, 0.005f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_height.png"), 10.0f, 0.2f, 0.02f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_normal_2.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_height.png"), 10.0f, 0.2f, 0.03f, 0.9f }
    };

    const std::shared_ptr<HeightMapInfo> heightMap = CreateHeightMap(heightGenerator);
    const std::shared_ptr<VertexData> vertexData = GenerateVertexData(heightMap, size);

    auto result = std::make_unique<TerrainComponent>(x, z);
    result->m_model = std::move(CreateModel(*allocator, vertexData, true));
    result->m_heightsInfo = CreateHeightMap(heightGenerator);
    result->m_vertexData = vertexData;
    for (const auto& layer : terrainLayers) {
        auto material = CreateMaterial(*allocator, layer.materialPath, layer.materialNormalPath, layer.materialHeightPath, layer.shineDamper, layer.reflectivity);
        material->SetHeightScale(layer.heightScale);
        result->m_materials.emplace_back(std::move(material));
        result->m_heightSteps.emplace_back(layer.heightStep);
    }
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<ITerrainComponenet> TerrainComponentFactory::CreateRandomTerrainConeStepMapped(const int x, const int z, const float size) const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();
    const auto heightGenerator = std::make_shared<HeightGenerator>(x, z, m_vertexCount, m_seed);

    const float layerTransitionWidth = 0.1f;
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_normal_2.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_cone.png"), 10.0f, 0.2f, 0.00002f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal_2.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_cone.png"), 10.0f, 0.2f, 0.01f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_cone.png"), 10.0f, 0.2f, 0.03f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_normal_2.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_cone.png"), 10.0f, 0.2f, 0.05f, 0.9f }
    };

    const std::shared_ptr<HeightMapInfo> heightMap = CreateHeightMap(heightGenerator);
    const std::shared_ptr<VertexData> vertexData = GenerateVertexData(heightMap, size);

    auto result = std::make_unique<TerrainComponent>(x, z);
    result->m_model = std::move(CreateModel(*allocator, vertexData, true));
    result->m_heightsInfo = CreateHeightMap(heightGenerator);
    result->m_vertexData = vertexData;
    for (const auto& layer : terrainLayers) {
        auto material = CreateMaterial(*allocator, layer.materialPath, layer.materialNormalPath, layer.materialHeightPath, layer.shineDamper, layer.reflectivity);
        material->SetHeightScale(layer.heightScale);
        result->m_materials.emplace_back(std::move(material));
        result->m_heightSteps.emplace_back(layer.heightStep);
    }
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::shared_ptr<prev::render::image::Image> TerrainComponentFactory::CreateImage(const std::string& textureFilename) const
{
    std::shared_ptr<prev::render::image::Image> image;
    if (s_terrainImageCache.find(textureFilename) != s_terrainImageCache.cend()) {
        image = s_terrainImageCache[textureFilename];
    } else {
        prev::render::image::ImageFactory imageFactory{};
        image = imageFactory.CreateImage(textureFilename);
        s_terrainImageCache[textureFilename] = image;
    }
    return image;
}

std::unique_ptr<prev_test::render::IModel> TerrainComponentFactory::CreateModel(prev::core::memory::Allocator& allocator, const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const
{
    auto mesh = GenerateMesh(vertexData, normalMapped);
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

    prev_test::render::model::ModelFactory modelFactory{};
    return modelFactory.Create(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IMaterial> TerrainComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const float shineDamper, const float reflectivity) const
{
    auto image = CreateImage(texturePath);
    auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)image->GetBuffer() });

    return prev_test::render::material::MaterialFactory{}.Create({ glm::vec3{ 1.0f }, shineDamper, reflectivity }, { image, std::move(imageBuffer) });
}

std::unique_ptr<prev_test::render::IMaterial> TerrainComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const std::string& normalMapPath, const float shineDamper, const float reflectivity) const
{
    auto image = CreateImage(texturePath);
    auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)image->GetBuffer() });

    auto normalImage = CreateImage(normalMapPath);
    auto normalImageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    normalImageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ normalImage->GetWidth(), normalImage->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)normalImage->GetBuffer() });

    return prev_test::render::material::MaterialFactory{}.Create({ glm::vec3{ 1.0f }, shineDamper, reflectivity }, { image, std::move(imageBuffer) }, { normalImage, std::move(normalImageBuffer) });
}

std::unique_ptr<prev_test::render::IMaterial> TerrainComponentFactory::CreateMaterial(prev::core::memory::Allocator& allocator, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightPath, const float shineDamper, const float reflectivity) const
{
    auto image = CreateImage(texturePath);
    auto imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)image->GetBuffer() });

    auto normalImage = CreateImage(normalMapPath);
    auto normalImageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    normalImageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ normalImage->GetWidth(), normalImage->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)normalImage->GetBuffer() });

    auto heightImage = CreateImage(heightPath);
    auto heightImageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(allocator);
    heightImageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ heightImage->GetWidth(), heightImage->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)heightImage->GetBuffer() });

    return prev_test::render::material::MaterialFactory{}.Create({ glm::vec3{ 1.0f }, shineDamper, reflectivity }, { image, std::move(imageBuffer) }, { normalImage, std::move(normalImageBuffer) }, { heightImage, std::move(heightImageBuffer) });
}

std::unique_ptr<prev_test::render::IMesh> TerrainComponentFactory::GenerateMesh(const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const
{
    auto mesh = std::make_unique<TerrainMesh>();
    mesh->m_indices = vertexData->indices;
    mesh->m_verticesCount = static_cast<uint32_t>(vertexData->vertices.size());
    if (normalMapped) {
        mesh->m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    } else {
        mesh->m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    }

    for (unsigned int z = 0; z < m_vertexCount; z++) {
        for (unsigned int x = 0; x < m_vertexCount; x++) {
            const auto vertexIndex = z * m_vertexCount + x;
            mesh->m_vertexDataBuffer.Add(vertexData->vertices[vertexIndex]);
            mesh->m_vertexDataBuffer.Add(vertexData->textureCoords[vertexIndex]);
            mesh->m_vertexDataBuffer.Add(vertexData->normals[vertexIndex]);
            mesh->m_vertices.push_back(vertexData->vertices[vertexIndex]);
            if (normalMapped) {
                mesh->m_vertexDataBuffer.Add(vertexData->tangents[vertexIndex]);
                mesh->m_vertexDataBuffer.Add(vertexData->biTangents[vertexIndex]);
            }
        }
    }

    mesh->m_meshParts.push_back(prev_test::render::MeshPart(static_cast<uint32_t>(vertexData->indices.size())));

    return mesh;
}

std::unique_ptr<VertexData> TerrainComponentFactory::GenerateVertexData(const std::shared_ptr<HeightMapInfo>& heightMap, const float size) const
{
    const auto verticesCount = m_vertexCount * m_vertexCount;
    auto result = std::make_unique<VertexData>();
    result->vertices.reserve(verticesCount);
    result->textureCoords.reserve(verticesCount);
    result->normals.reserve(verticesCount);
    for (unsigned int z = 0; z < m_vertexCount; z++) {
        for (unsigned int x = 0; x < m_vertexCount; x++) {
            result->vertices.push_back(CalculatePosition(heightMap, x, z, size));
            result->textureCoords.push_back(CalculateTextureCoordinates(x, z));
            result->normals.push_back(CalculateNormal(heightMap, x, z));
        }
    }

    const auto indicesCount = 6 * (m_vertexCount - 1) * (m_vertexCount - 1);
    result->indices.reserve(indicesCount);
    for (unsigned int z = 0; z < m_vertexCount - 1; z++) {
        for (unsigned int x = 0; x < m_vertexCount - 1; x++) {
            const uint32_t topLeft = (z * m_vertexCount) + x;
            const uint32_t topRight = topLeft + 1;
            const uint32_t bottomLeft = ((z + 1) * m_vertexCount) + x;
            const uint32_t bottomRight = bottomLeft + 1;

            result->indices.push_back(topLeft);
            result->indices.push_back(bottomLeft);
            result->indices.push_back(topRight);
            result->indices.push_back(topRight);
            result->indices.push_back(bottomLeft);
            result->indices.push_back(bottomRight);
        }
    }

    prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(result->vertices, result->textureCoords, result->indices, result->tangents, result->biTangents);

    return result;
}

glm::vec3 TerrainComponentFactory::CalculatePosition(const std::shared_ptr<HeightMapInfo>& heightMap, const int x, const int z, const float size) const
{
    glm::vec3 result{};
    result.x = (static_cast<float>(x) / static_cast<float>(m_vertexCount - 1.0f)) * size;
    result.y = heightMap->GetHeightAt(x, z);
    result.z = (static_cast<float>(z) / static_cast<float>(m_vertexCount - 1.0f)) * size;
    return result;
}

glm::vec2 TerrainComponentFactory::CalculateTextureCoordinates(const int x, const int z) const
{
    glm::vec2 result{};
    result.x = static_cast<float>(x) / static_cast<float>(m_vertexCount) - 1.0f;
    result.y = static_cast<float>(z) / static_cast<float>(m_vertexCount) - 1.0f;
    return result;
}

glm::vec3 TerrainComponentFactory::CalculateNormal(const std::shared_ptr<HeightMapInfo>& heightMap, const int x, const int z) const
{
    const float heightLeft = heightMap->GetHeightAt(x - 1, z);
    const float heightRight = heightMap->GetHeightAt(x + 1, z);
    const float heightBottom = heightMap->GetHeightAt(x, z - 1);
    const float heightTop = heightMap->GetHeightAt(x, z + 1);

    glm::vec3 result(heightLeft - heightRight, 2.0f, heightBottom - heightTop);
    result = glm::normalize(result);
    return result;
}

std::unique_ptr<HeightMapInfo> TerrainComponentFactory::CreateHeightMap(const std::shared_ptr<HeightGenerator>& generator) const
{
    float minHeight = std::numeric_limits<float>::max();
    float maxHeight = std::numeric_limits<float>::min();
    auto heightMapInfo = std::make_unique<HeightMapInfo>(m_vertexCount);
    for (unsigned int z = 0; z < m_vertexCount; z++) {
        for (unsigned int x = 0; x < m_vertexCount; x++) {
            float height = generator->GenerateHeight(x, z);
            heightMapInfo->heights[x][z] = height;
            minHeight = std::min(minHeight, height);
            maxHeight = std::max(maxHeight, height);
        }
    }
    heightMapInfo->minHeight = heightMapInfo->globalMinHeight = minHeight;
    heightMapInfo->maxHeight = heightMapInfo->globalMaxHeight = maxHeight;
    return heightMapInfo;
}
} // namespace prev_test::component::terrain