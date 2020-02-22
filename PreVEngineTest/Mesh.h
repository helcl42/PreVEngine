#ifndef __MESH_H__
#define __MESH_H__

#include <Utils.h>

enum class VertexLayoutComponent {
    FLOAT = 0x0,
    VEC2 = 0x1,
    VEC3 = 0x2,
    VEC4 = 0x3
};

struct VertexLayout {
private:
    std::vector<VertexLayoutComponent> m_components;

public:
    VertexLayout()
    {
    }

    VertexLayout(const std::vector<VertexLayoutComponent>& components)
        : m_components(components)
    {
    }

public:
    const std::vector<VertexLayoutComponent>& GetComponents() const
    {
        return m_components;
    }

    uint32_t GetStride() const
    {
        uint32_t singleVertexPackSizeInBytes = 0;
        for (auto& component : m_components) {
            switch (component) {
            case VertexLayoutComponent::FLOAT:
                singleVertexPackSizeInBytes += 1 * sizeof(float);
                break;
            case VertexLayoutComponent::VEC2:
                singleVertexPackSizeInBytes += 2 * sizeof(float);
                break;
            case VertexLayoutComponent::VEC4:
                singleVertexPackSizeInBytes += 4 * sizeof(float);
                break;
            default:
                singleVertexPackSizeInBytes += 3 * sizeof(float);
                break;
            }
        }
        return singleVertexPackSizeInBytes;
    }
};

class VertexDataBuffer {
private:
    std::vector<uint8_t> m_buffer;

public:
    VertexDataBuffer()
    {
    }

    VertexDataBuffer(const size_t initialSize)
    {
        m_buffer.reserve(initialSize);
    }

public:
    void Add(const void* data, const unsigned int size)
    {
        m_buffer.insert(m_buffer.end(), (const uint8_t*)data, (const uint8_t*)data + size);
    }

    void Add(float data)
    {
        Add(&data, sizeof(float));
    }

    void Add(const glm::vec2& data)
    {
        Add(&data, sizeof(glm::vec2));
    }

    void Add(const glm::vec3& data)
    {
        Add(&data, sizeof(glm::vec3));
    }

    void Add(const glm::vec4& data)
    {
        Add(&data, sizeof(glm::vec4));
    }

    void Reset()
    {
        m_buffer.clear();
    }

    const uint8_t* GetData() const
    {
        return m_buffer.data();
    }
};

class IMesh {
public:
    virtual const VertexLayout& GetVertextLayout() const = 0;

    virtual const void* GetVertices() const = 0;

    virtual uint32_t GerVerticesCount() const = 0;

    virtual const std::vector<uint32_t>& GerIndices() const = 0;

    virtual bool HasIndices() const = 0;

public:
    virtual ~IMesh()
    {
    }
};

class CubeMesh : public IMesh {
private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tc;
        glm::vec3 normal;
    };

private:
    const VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    const std::vector<Vertex> m_vertices = {
        // FROMT
        { { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },

        // BACK
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },

        // TOP
        { { -0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },

        // BOTTOM
        { { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },

        // LEFT
        { { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },

        // RIGHT
        { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } }
    };

    const std::vector<uint32_t> m_indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return (const void*)m_vertices.data();
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(m_vertices.size());
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return m_indices;
    }

    bool HasIndices() const override
    {
        return true;
    }
};

class QuadMesh : public IMesh {
private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tc;
        glm::vec3 normal;
    };

private:
    const VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    const std::vector<Vertex> m_vertices = {
        { { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
    };

    const std::vector<uint32_t> m_indices = {
        0, 1, 2, 2, 3, 0
    };

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return (const void*)m_vertices.data();
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(m_vertices.size());
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return m_indices;
    }

    bool HasIndices() const override
    {
        return true;
    }
};

class PlaneMesh : public IMesh {
private:
    const VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    VertexDataBuffer m_vertexDataBuffer;

    uint32_t m_verticesCount = 0;

    std::vector<uint32_t> m_indices;

public:
    PlaneMesh(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float sMax = 1.0f, const float tMax = 1.0f)
    {
        const float x2 = xSize / 2.0f;
        const float z2 = zSize / 2.0f;
        const float iFactor = (float)zSize / zDivs;
        const float jFactor = (float)xSize / xDivs;
        const float texi = (float)sMax / zDivs;
        const float texj = (float)tMax / xDivs;

        for (uint32_t i = 0; i <= zDivs; i++) {
            float z = iFactor * i - z2;

            for (uint32_t j = 0; j <= xDivs; j++) {
                float x = jFactor * j - x2;

                glm::vec3 vertex(x, 0.0f, z);
                glm::vec2 textureCoord(j * texi, i * texj);
                glm::vec3 normal(0.0f, 1.0f, 0.0f);

                m_vertexDataBuffer.Add(vertex);
                m_vertexDataBuffer.Add(textureCoord);
                m_vertexDataBuffer.Add(normal);
                m_verticesCount++;
            }
        }

        for (uint32_t i = 0; i < zDivs; i++) {
            const uint32_t rowStart = i * (xDivs + 1);
            const uint32_t nextRowStart = (i + 1) * (xDivs + 1);

            for (uint32_t j = 0; j < xDivs; j++) {
                const uint32_t indices[] = {
                    rowStart + j,
                    nextRowStart + j,
                    nextRowStart + j + 1,
                    rowStart + j,
                    nextRowStart + j + 1,
                    rowStart + j + 1
                };

                for (const auto index : indices) {
                    m_indices.emplace_back(index);
                }
            }
        }
    }

    virtual ~PlaneMesh()
    {
    }

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
        return true;
    }
};

#endif