#include "TerrainComponentFactory.h"
#include "HeightGenerator.h"
#include "TerrainComponent.h"
#include "TerrainMesh.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshUtil.h"
#include "../../render/model/ModelFactory.h"

namespace prev_test::component::terrain {
TerrainComponentFactory::TerrainComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const unsigned int seed, const unsigned int vertexCount)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_seed{ seed }
    , m_vertexCount{ vertexCount }
{
}

std::unique_ptr<ITerrainComponenet> TerrainComponentFactory::CreateRandomTerrain(const int x, const int z, const float size) const
{
    HeightGenerator heightGenerator(x, z, m_vertexCount, m_seed);
    const std::shared_ptr<HeightMapInfo> heightMap{ CreateHeightMap(heightGenerator) };
    const std::shared_ptr<VertexData> vertexData{ GenerateVertexData(heightMap, size) };

    prev_test::render::material::MaterialFactory materialFactory{ m_device, m_allocator };

    const float layerTransitionWidth{ 0.1f };
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), 20.0f, 0.05f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), 20.0f, 0.05f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), 20.0f, 0.05f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), 20.0f, 0.05f, 0.9f }
    };

    auto result{ std::make_unique<TerrainComponent>(x, z) };
    result->m_model = CreateModel(vertexData, false);
    result->m_heightsInfo = heightMap;
    result->m_vertexData = vertexData;
    for (const auto& layer : terrainLayers) {
        result->m_materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), layer.shineDamper, layer.reflectivity, VK_SAMPLER_ADDRESS_MODE_REPEAT }, layer.materialPath));
        result->m_heightSteps.emplace_back(layer.heightStep);
    }
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<ITerrainComponenet> TerrainComponentFactory::CreateRandomTerrainNormalMapped(const int x, const int z, const float size) const
{
    HeightGenerator heightGenerator(x, z, m_vertexCount, m_seed);
    const std::shared_ptr<HeightMapInfo> heightMap{ CreateHeightMap(heightGenerator) };
    const std::shared_ptr<VertexData> vertexData{ GenerateVertexData(heightMap, size) };

    prev_test::render::material::MaterialFactory materialFactory{ m_device, m_allocator };

    const float layerTransitionWidth{ 0.1f };
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_normal.png"), 20.0f, 0.05f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal.png"), 20.0f, 0.05f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), 20.0f, 0.05f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_normal.png"), 20.0f, 0.05f, 0.9f }
    };

    auto result{ std::make_unique<TerrainComponent>(x, z) };
    result->m_model = CreateModel(vertexData, true);
    result->m_heightsInfo = heightMap;
    result->m_vertexData = vertexData;
    for (const auto& layer : terrainLayers) {
        result->m_materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), layer.shineDamper, layer.reflectivity, VK_SAMPLER_ADDRESS_MODE_REPEAT }, layer.materialPath, layer.materialNormalPath));
        result->m_heightSteps.emplace_back(layer.heightStep);
    }
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<ITerrainComponenet> TerrainComponentFactory::CreateRandomTerrainConeStepMapped(const int x, const int z, const float size) const
{
    HeightGenerator heightGenerator(x, z, m_vertexCount, m_seed);
    const std::shared_ptr<HeightMapInfo> heightMap{ CreateHeightMap(heightGenerator) };
    const std::shared_ptr<VertexData> vertexData{ GenerateVertexData(heightMap, size) };

    prev_test::render::material::MaterialFactory materialFactory{ m_device, m_allocator };

    const float layerTransitionWidth{ 0.1f };
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_cone.png"), 20.0f, 0.05f, 0.00002f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_cone.png"), 20.0f, 0.05f, 0.01f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_cone.png"), 20.0f, 0.05f, 0.03f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_cone.png"), 20.0f, 0.05f, 0.05f, 0.9f }
    };

    auto result{ std::make_unique<TerrainComponent>(x, z) };
    result->m_model = CreateModel(vertexData, true);
    result->m_heightsInfo = heightMap;
    result->m_vertexData = vertexData;
    for (const auto& layer : terrainLayers) {
        auto material{ materialFactory.Create({ glm::vec4(1.0f), layer.shineDamper, layer.reflectivity, VK_SAMPLER_ADDRESS_MODE_REPEAT }, layer.materialPath, layer.materialNormalPath, layer.materialHeightPath) };
        material->SetHeightScale(layer.heightScale);
        result->m_materials.emplace_back(std::move(material));
        result->m_heightSteps.emplace_back(layer.heightStep);
    }
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<prev_test::render::IModel> TerrainComponentFactory::CreateModel(const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const
{
    auto mesh{ GenerateMesh(vertexData, normalMapped) };
    auto model{ prev_test::render::model::ModelFactory{ m_allocator }.Create(std::move(mesh)) };
    return model;
}

std::unique_ptr<prev_test::render::IMesh> TerrainComponentFactory::GenerateMesh(const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const
{
    auto mesh{ std::make_unique<TerrainMesh>() };
    mesh->m_indices = vertexData->indices;
    mesh->m_verticesCount = static_cast<uint32_t>(vertexData->vertices.size());
    if (normalMapped) {
        mesh->m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    } else {
        mesh->m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    }

    for (uint32_t z = 0; z < m_vertexCount; ++z) {
        for (uint32_t x = 0; x < m_vertexCount; ++x) {
            const auto vertexIndex{ z * m_vertexCount + x };
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

    mesh->m_meshParts.emplace_back(prev_test::render::MeshPart(static_cast<uint32_t>(vertexData->indices.size()), vertexData->vertices));
    mesh->m_meshRootNode = prev_test::render::MeshNode{ { 0 }, glm::mat4(1.0f), {} };

    return mesh;
}

std::unique_ptr<VertexData> TerrainComponentFactory::GenerateVertexData(const std::shared_ptr<HeightMapInfo>& heightMap, const float size) const
{
    const auto verticesCount{ m_vertexCount * m_vertexCount };
    auto result{ std::make_unique<VertexData>() };
    result->vertices.reserve(verticesCount);
    result->textureCoords.reserve(verticesCount);
    for (uint32_t z = 0; z < m_vertexCount; ++z) {
        for (uint32_t x = 0; x < m_vertexCount; ++x) {
            result->vertices.push_back(CalculatePosition(heightMap, x, z, size));
            result->textureCoords.push_back(CalculateTextureCoordinates(x, z));
        }
    }

    const auto indicesCount{ 6 * (m_vertexCount - 1) * (m_vertexCount - 1) };
    result->indices.reserve(indicesCount);
    for (uint32_t z = 0; z < m_vertexCount - 1; ++z) {
        for (uint32_t x = 0; x < m_vertexCount - 1; ++x) {
            const uint32_t bottomLeft{ (z * m_vertexCount) + x };
            const uint32_t bottomRight{ bottomLeft + 1 };
            const uint32_t topLeft{ ((z + 1) * m_vertexCount) + x };
            const uint32_t topRight{ topLeft + 1 };

            result->indices.push_back(topLeft);
            result->indices.push_back(topRight);
            result->indices.push_back(bottomRight);
            result->indices.push_back(bottomRight);
            result->indices.push_back(bottomLeft);
            result->indices.push_back(topLeft);
        }
    }

    result->normals = prev_test::render::mesh::MeshUtil::GenerateNormals(result->vertices, result->indices, true);

    std::tie(result->tangents, result->biTangents) = prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(result->vertices, result->textureCoords, result->normals, result->indices);

    return result;
}

glm::vec3 TerrainComponentFactory::CalculatePosition(const std::shared_ptr<HeightMapInfo>& heightMap, const int x, const int z, const float size) const
{
    glm::vec3 result{};
    result.x = (static_cast<float>(x) / static_cast<float>(m_vertexCount - 1)) * size;
    result.y = heightMap->GetHeightAt(x, z);
    result.z = (static_cast<float>(z) / static_cast<float>(m_vertexCount - 1)) * size;
    return result;
}

glm::vec2 TerrainComponentFactory::CalculateTextureCoordinates(const int x, const int z) const
{
    glm::vec2 result{};
    result.x = 2.0f * (static_cast<float>(x) / static_cast<float>(m_vertexCount - 1)) - 1.0f;
    result.y = 2.0f * (static_cast<float>(z) / static_cast<float>(m_vertexCount - 1)) - 1.0f;
    return result;
}

std::unique_ptr<HeightMapInfo> TerrainComponentFactory::CreateHeightMap(const HeightGenerator& generator) const
{
    float minHeight{ std::numeric_limits<float>::max() };
    float maxHeight{ -std::numeric_limits<float>::max() };
    auto heightMapInfo{ std::make_unique<HeightMapInfo>(m_vertexCount) };
    for (unsigned int z = 0; z < m_vertexCount; ++z) {
        for (unsigned int x = 0; x < m_vertexCount; ++x) {
            const float height{ generator.GenerateHeight(x, z) };
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