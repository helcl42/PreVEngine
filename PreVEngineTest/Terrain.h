#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "General.h"

#include <vector>

static const float TERRAIN_SIZE{ 240.0f };

class PerlinNoiseGenerator {
public:
    explicit PerlinNoiseGenerator()
        : m_seed(GenerateSeed())
    {
    }

    explicit PerlinNoiseGenerator(const unsigned int seed)
        : m_seed(seed)
    {
    }

    virtual ~PerlinNoiseGenerator() = default;

public:
    float GetInterpolatedNoise(const float x, const float z)
    {
        int intX = static_cast<int>(x);
        int intZ = static_cast<int>(z);
        float fracX = x - intX;
        float fracZ = z - intZ;

        float v1 = GetSmoothNoise(intX, intZ);
        float v2 = GetSmoothNoise(intX + 1, intZ);
        float v3 = GetSmoothNoise(intX, intZ + 1);
        float v4 = GetSmoothNoise(intX + 1, intZ + 1);

        float i1 = Interpolate(v1, v2, fracX);
        float i2 = Interpolate(v3, v4, fracX);
        return Interpolate(i1, i2, fracZ);
    }

private:
    static unsigned int GenerateSeed()
    {
        std::random_device r;
        std::default_random_engine eng{ r() };
        std::uniform_real_distribution<float> urd(0, 1000000000);
        return static_cast<unsigned int>(urd(eng));
    }

    static float Interpolate(const float a, const float b, const float blend)
    {
        const float theta = blend * glm::pi<float>();
        const float factor = (1.0f - cosf(theta)) * 0.5f;
        return a * (1.0f - factor) + b * factor;
    }

    float GetNoise(const int x, const int z) const
    {
        const unsigned int seed = x * 49632 + z * 325176 + m_seed;
        std::default_random_engine eng{ static_cast<long unsigned int>(seed) };
        std::uniform_real_distribution<float> urd(-1.0, 1.0);
        return urd(eng);
    }

    float GetSmoothNoise(const int x, const int z) const
    {
        const float corners = (GetNoise(x - 1, z - 1) + GetNoise(x + 1, z - 1) + GetNoise(x - 1, z + 1) + GetNoise(x + 1, z + 1)) / 16.0f;
        const float sides = (GetNoise(x - 1, z) + GetNoise(x + 1, z) + GetNoise(x, z - 1) + GetNoise(x, z + 1)) / 8.0f;
        const float center = GetNoise(x, z) / 4.0f;
        return corners + sides + center;
    }

private:
    const unsigned int m_seed;
};

class HeightGenerator {
public:
    inline static const float AMPLITUDE{ 60.0f };

    inline static const int OCTAVES{ 5 };

    inline static const float ROUGHNESS{ 0.1f };

public:
    explicit HeightGenerator()
        : m_xOffset(0)
        , m_zOffset(0)
        , m_noiseGenerator(std::make_shared<PerlinNoiseGenerator>())
    {
    }

    explicit HeightGenerator(const int x, const int z, const int size, const unsigned int seed)
        : m_xOffset(x * (size - 1))
        , m_zOffset(z * (size - 1))
        , m_noiseGenerator(std::make_shared<PerlinNoiseGenerator>(seed))
    {
    }

    ~HeightGenerator() = default;

public:
    float GenerateHeight(const int x, const int z) const
    {
        const float d = static_cast<float>(powf(2.0f, static_cast<float>(HeightGenerator::OCTAVES - 1)));

        float total{ 0.0f };
        for (int i = 0; i < HeightGenerator::OCTAVES; i++) {
            const float freq = static_cast<float>(powf(2, static_cast<float>(i)) / d);
            const float amp = static_cast<float>(powf(HeightGenerator::ROUGHNESS, static_cast<float>(i))) * static_cast<float>(HeightGenerator::AMPLITUDE);
            total += m_noiseGenerator->GetInterpolatedNoise((x + m_xOffset) * freq, (z + m_zOffset) * freq) * amp;
        }
        return total;
    }

private:
    const int m_xOffset;

    const int m_zOffset;

    const std::shared_ptr<PerlinNoiseGenerator> m_noiseGenerator;
};

struct HeightMapInfo {
    std::vector<std::vector<float> > heights;

    float minHeight{ 0.0f };

    float maxHeight{ 0.0f };

    HeightMapInfo() = default;

    HeightMapInfo(const size_t size)
    {
        heights.resize(size);
        for (size_t i = 0; i < size; i++) {
            heights[i] = std::vector<float>(size);
        }
    }

    float GetHeightAt(const uint32_t x, const uint32_t z) const
    {
        return heights[x][z];
    }

    size_t GetSize() const
    {
        return heights.size();
    }

    float GetMinHeight() const
    {
        return minHeight;
    }

    float GetMaxHeight() const
    {
        return maxHeight;
    }
};

class ITerrainComponenet : public IBasicRenderComponent {
public:
    virtual std::vector<std::shared_ptr<IMaterial> > GetMaterials() const = 0; // TODO make pack of materials controlled by height

    virtual float GetHeightAt(const glm::vec3& position) const = 0;

    virtual std::shared_ptr<VertexData> GetVertexData() const = 0;

    virtual std::shared_ptr<HeightMapInfo> GetHeightMapInfo() const = 0;

    virtual const glm::vec3& GetPosition() const = 0;

    virtual int GetGridX() const = 0;

    virtual int GetGridZ() const = 0;

    virtual std::vector<float> GetHeightSteps() const = 0;

    virtual float GetTransitionRange() const = 0;

public:
    virtual ~ITerrainComponenet() = default;
};

class TerrainComponentFactory;

class TerrainComponent : public ITerrainComponenet {
public:
    TerrainComponent(const int gridX, const int gridZ)
        : m_gridX(gridX)
        , m_gridZ(gridZ)
        , m_position(glm::vec3(gridX * TERRAIN_SIZE, 0.0f, gridZ * TERRAIN_SIZE))
    {
    }

    ~TerrainComponent() = default;

public:
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

    std::vector<std::shared_ptr<IMaterial> > GetMaterials() const override
    {
        return m_materials;
    }

    float GetHeightAt(const glm::vec3& position) const override
    {
        const float terrainX = position.x - m_position.x;
        const float terrainZ = position.z - m_position.z;
        const float gridSquareSize = TERRAIN_SIZE / (static_cast<float>(m_heightsInfo->GetSize()) - 1.0f);
        const int gridX = static_cast<int>(floorf(terrainX / gridSquareSize));
        const int gridZ = static_cast<int>(floorf(terrainZ / gridSquareSize));

        if (gridX >= static_cast<int>(m_heightsInfo->GetSize()) - 1 || gridX < 0 || gridZ >= static_cast<int>(m_heightsInfo->GetSize()) - 1 || gridZ < 0) {
            return 0.0f;
        }

        const float xCoord = fmodf(terrainX, gridSquareSize) / gridSquareSize;
        const float zCoord = fmodf(terrainZ, gridSquareSize) / gridSquareSize;

        float answer;
        if (xCoord <= (1 - zCoord)) {
            answer = MathUtil::BarryCentric(glm::vec3(0, m_heightsInfo->GetHeightAt(gridX, gridZ), 0), glm::vec3(1, m_heightsInfo->GetHeightAt(gridX + 1, gridZ), 0), glm::vec3(0, m_heightsInfo->GetHeightAt(gridX, gridZ + 1), 1), glm::vec2(xCoord, zCoord));
        } else {
            answer = MathUtil::BarryCentric(glm::vec3(1, m_heightsInfo->GetHeightAt(gridX + 1, gridZ), 0), glm::vec3(1, m_heightsInfo->GetHeightAt(gridX + 1, gridZ + 1), 1), glm::vec3(0, m_heightsInfo->GetHeightAt(gridX, gridZ + 1), 1), glm::vec2(xCoord, zCoord));
        }
        return answer;
    }

    std::shared_ptr<VertexData> GetVertexData() const override
    {
        return m_vertexData;
    }

    const glm::vec3& GetPosition() const override
    {
        return m_position;
    }

    std::shared_ptr<HeightMapInfo> GetHeightMapInfo() const override
    {
        return m_heightsInfo;
    }

    int GetGridX() const override
    {
        return m_gridX;
    }

    int GetGridZ() const override
    {
        return m_gridZ;
    }

    std::vector<float> GetHeightSteps() const override
    {
        return m_heightSteps;
    }

    float GetTransitionRange() const override
    {
        return m_transitionRange;
    }

private:
    friend class TerrainComponentFactory;

private:
    const int m_gridX;

    const int m_gridZ;

    const glm::vec3 m_position;

    std::shared_ptr<HeightMapInfo> m_heightsInfo;

    std::shared_ptr<VertexData> m_vertexData;

    std::shared_ptr<IModel> m_model;

    std::vector<std::shared_ptr<IMaterial> > m_materials;

    std::vector<float> m_heightSteps;

    float m_transitionRange;
};

class TerrainMesh : public IMesh {
private:
    friend TerrainComponentFactory;

private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    uint32_t m_verticesCount{ 0 };

    std::vector<uint32_t> m_indices;

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    uint32_t GerVerticesCount() const override
    {
        return m_verticesCount;
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return m_indices;
    }

    bool HasIndices() const override
    {
        return m_indices.size() > 0;
    }
};

class TerrainComponentFactory {
public:
    TerrainComponentFactory(const unsigned int seed = 21236728, const unsigned int vertexCount = 128)
        : m_seed(seed)
        , m_vertexCount(vertexCount)
    {
    }

    ~TerrainComponentFactory() = default;

public:
    std::unique_ptr<ITerrainComponenet> CreateRandomTerrain(const int x, const int z, const float size) const
    {
        auto allocator = AllocatorProvider::GetInstance().GetAllocator();
        const auto heightGenerator = std::make_shared<HeightGenerator>(x, z, m_vertexCount, m_seed);

        const std::string materialPaths[] = {
            "fungus.png",
            "sand_grass.png",
            "rock.png",
            "sand.png"
        };

        auto result = std::make_unique<TerrainComponent>(x, z);
        result->m_model = std::move(CreateModel(*allocator, heightGenerator, size));
        result->m_heightsInfo = CreateHeightMap(heightGenerator);
        result->m_vertexData = GenerateVertexData(heightGenerator, size);
        for (const auto& path : materialPaths) {
            auto material = CreateMaterial(*allocator, path, 3.0f, 0.4f);
            result->m_materials.emplace_back(std::move(material));
        }
        result->m_heightSteps = { 0.2f, 0.4f, 0.6f, 0.8f };
        result->m_transitionRange = 0.1f;
        return result;
    }

private:
    std::unique_ptr<IModel> CreateModel(Allocator& allocator, const std::shared_ptr<HeightGenerator>& heightGenerator, const float size) const
    {
        auto mesh = GenerateMesh(heightGenerator, size);
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertices(), mesh->GerVerticesCount(), mesh->GetVertextLayout().GetStride());
        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GerIndices().data(), static_cast<uint32_t>(mesh->GerIndices().size()));

        return std::make_unique<Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::string& textureFilename, const float shineDamper, const float reflectivity) const
    {
        ImageFactory imageFactory;
        auto image = imageFactory.CreateImage(textureFilename);
        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ { image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT, (uint8_t*)image->GetBuffer() });

        return std::make_unique<Material>(std::move(image), std::move(imageBuffer), shineDamper, reflectivity);
    }

    std::unique_ptr<IMesh> GenerateMesh(const std::shared_ptr<HeightGenerator>& generator, const float size) const
    {
        auto mesh = std::make_unique<TerrainMesh>();
        mesh->m_indices = GenerateIndices();
        mesh->m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        for (unsigned int z = 0; z < m_vertexCount; z++) {
            for (unsigned int x = 0; x < m_vertexCount; x++) {
                auto position = CalculatePosition(generator, x, z, size);
                auto texCoord = CalculateTextureCoordinates(x, z);
                auto normal = CalculateNormal(generator, x, z);
                mesh->m_vertexDataBuffer.Add(&position, sizeof(glm::vec3));
                mesh->m_vertexDataBuffer.Add(&texCoord, sizeof(glm::vec2));
                mesh->m_vertexDataBuffer.Add(&normal, sizeof(glm::vec3));
                mesh->m_verticesCount++;
            }
        }
        return mesh;
    }

    std::unique_ptr<VertexData> GenerateVertexData(const std::shared_ptr<HeightGenerator>& generator, const float size) const
    {
        auto result = std::make_unique<VertexData>(m_vertexCount * m_vertexCount);
        for (unsigned int z = 0; z < m_vertexCount; z++) {
            for (unsigned int x = 0; x < m_vertexCount; x++) {
                result->vertices.push_back(CalculatePosition(generator, x, z, size));
                result->textureCoords.push_back(CalculateTextureCoordinates(x, z));
                result->normals.push_back(CalculateNormal(generator, x, z));
            }
        }
        return result;
    }

    std::vector<uint32_t> GenerateIndices() const
    {
        const auto indicesCount = 6 * (m_vertexCount - 1) * (m_vertexCount - 1);

        std::vector<uint32_t> result{};
        result.reserve(indicesCount);
        for (unsigned int z = 0; z < m_vertexCount - 1; z++) {
            for (unsigned int x = 0; x < m_vertexCount - 1; x++) {
                const uint32_t topLeft = (z * m_vertexCount) + x;
                const uint32_t topRight = topLeft + 1;
                const uint32_t bottomLeft = ((z + 1) * m_vertexCount) + x;
                const uint32_t bottomRight = bottomLeft + 1;

                result.push_back(topLeft);
                result.push_back(bottomLeft);
                result.push_back(topRight);
                result.push_back(topRight);
                result.push_back(bottomLeft);
                result.push_back(bottomRight);
            }
        }
        return result;
    }

    glm::vec3 CalculatePosition(const std::shared_ptr<HeightGenerator>& generator, const int x, const int z, const float size) const
    {
        glm::vec3 result{};
        result.x = (static_cast<float>(x) / (static_cast<float>(m_vertexCount) - 1.0f)) * size;
        result.y = generator->GenerateHeight(x, z);
        result.z = (static_cast<float>(z) / (static_cast<float>(m_vertexCount) - 1.0f)) * size;
        return result;
    }

    glm::vec2 CalculateTextureCoordinates(const int x, const int z) const
    {
        glm::vec2 result{};
        result.x = static_cast<float>(x) / (static_cast<float>(m_vertexCount) - 1.0f);
        result.y = static_cast<float>(z) / (static_cast<float>(m_vertexCount) - 1.0f);
        return result;
    }

    glm::vec3 CalculateNormal(const std::shared_ptr<HeightGenerator>& generator, const int x, const int z) const
    {
        float heightLeft = generator->GenerateHeight(x - 1, z);
        float heightRight = generator->GenerateHeight(x + 1, z);
        float heightBottom = generator->GenerateHeight(x, z - 1);
        float heightTop = generator->GenerateHeight(x, z + 1);

        glm::vec3 result(heightLeft - heightRight, 2.0f, heightBottom - heightTop);
        result = glm::normalize(result);
        return result;
    }

    std::unique_ptr<HeightMapInfo> CreateHeightMap(const std::shared_ptr<HeightGenerator>& generator) const
    {
        float minHeight = std::numeric_limits<float>::max();
        float maxHeight = std::numeric_limits<float>::min();
        auto heightMapInfo = std::make_unique<HeightMapInfo>(m_vertexCount);
        for (unsigned int z = 0; z < m_vertexCount; z++) {
            for (unsigned int x = 0; x < m_vertexCount; x++) {
                float height = generator->GenerateHeight(x, z);
                heightMapInfo->heights[x][z] = height;
                if (minHeight > height) {
                    minHeight = height;
                }
                if (maxHeight < height) {
                    maxHeight = height;
                }
            }
        }
        heightMapInfo->minHeight = minHeight;
        heightMapInfo->maxHeight = maxHeight;
        return heightMapInfo;
    }

private:
    const unsigned int m_seed;

    const unsigned int m_vertexCount;
};

struct TerrainKey {
    const int xIndex;

    const int zIndex;

    explicit TerrainKey(const int x, const int z)
        : xIndex(x)
        , zIndex(z)
    {
    }

    explicit TerrainKey(const glm::vec3& position)
        : xIndex(static_cast<int>(position.x / TERRAIN_SIZE))
        , zIndex(static_cast<int>(position.z / TERRAIN_SIZE))
    {
    }

    ~TerrainKey() = default;

    friend bool operator<(const TerrainKey& a, const TerrainKey& b)
    {
        if (a.xIndex < b.xIndex) {
            return true;
        }
        if (a.xIndex > b.xIndex) {
            return false;
        }

        if (a.zIndex < b.zIndex) {
            return true;
        }
        if (a.zIndex > b.zIndex) {
            return false;
        }

        return false;
    }
};

class ITerrainManagerComponent {
public:
    virtual void AddTerrainComponent(const std::shared_ptr<ITerrainComponenet>& terrain) = 0;

    virtual void RemoveTerrain(const std::shared_ptr<ITerrainComponenet>& terrain) = 0;

    virtual std::shared_ptr<ITerrainComponenet> GetTerrainAt(const glm::vec3& position) const = 0;

    virtual float GetHeightAt(const glm::vec3& position) const = 0;

public:
    virtual ~ITerrainManagerComponent() = default;
};

class TerrainManagerComponent : public ITerrainManagerComponent {
public:
    void AddTerrainComponent(const std::shared_ptr<ITerrainComponenet>& terrain) override
    {
        m_terrains.insert({ TerrainKey{ terrain->GetPosition() }, terrain });
    }

    void RemoveTerrain(const std::shared_ptr<ITerrainComponenet>& terrain) override
    {
        m_terrains.erase(TerrainKey{ terrain->GetPosition() });
    }

    std::shared_ptr<ITerrainComponenet> GetTerrainAt(const glm::vec3& position) const override
    {
        const TerrainKey key{ position };
        if (m_terrains.find(key) != m_terrains.cend()) {
            return m_terrains.at(key).lock();
        }
        return nullptr;
    }

    float GetHeightAt(const glm::vec3& position) const override
    {
        const auto terrain = GetTerrainAt(position);
        if (terrain) {
            return terrain->GetHeightAt(position);
        }
        return 0.0f;
    }

private:
    std::map<TerrainKey, std::weak_ptr<ITerrainComponenet> > m_terrains;
};

class TerrainManagerComponentFactory {
public:
    std::unique_ptr<ITerrainManagerComponent> Create() const
    {
        return std::make_unique<TerrainManagerComponent>();
    }
};

#endif // !__TERRAIN_H__
