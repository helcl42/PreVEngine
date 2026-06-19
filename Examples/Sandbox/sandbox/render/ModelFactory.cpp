#include "ModelFactory.h"

#include <prev/render/buffer/BufferBuilder.h>

#include <array>

namespace sandbox::render {
namespace {
    const std::array<Vertex, 24> CUBE_VERTICES{ {
        // FRONT (+Z)
        { { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        // BACK (-Z)
        { { -0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f } },
        // TOP (+Y)
        { { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
        // BOTTOM (-Y)
        { { -0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f } },
        // LEFT (-X)
        { { -0.5f, -0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, 0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f } },
        // RIGHT (+X)
        { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } },
    } };

    std::array<uint32_t, 36> MakeCubeIndices()
    {
        std::array<uint32_t, 36> indices{};
        for (uint32_t face = 0; face < 6; ++face) {
            const uint32_t v{ face * 4 };
            const uint32_t i{ face * 6 };
            indices[i + 0] = v + 0;
            indices[i + 1] = v + 1;
            indices[i + 2] = v + 2;
            indices[i + 3] = v + 2;
            indices[i + 4] = v + 3;
            indices[i + 5] = v + 0;
        }
        return indices;
    }
} // namespace

std::unique_ptr<Model> ModelFactory::CreateSandbox(prev::core::device::Device& device)
{
    const auto indices{ MakeCubeIndices() };

    auto model{ std::make_unique<Model>() };
    model->indexCount = static_cast<uint32_t>(indices.size());

    const uint64_t verticesSize{ sizeof(CUBE_VERTICES) };
    model->vertexBuffer = prev::render::buffer::BufferBuilder{ device, device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                              .SetUsageFlags(GFX_BUFFER_USAGE_VERTEX | GFX_BUFFER_USAGE_COPY_DST)
                              .SetMemoryProperties(GFX_MEMORY_PROPERTY_DEVICE_LOCAL)
                              .SetSize(verticesSize)
                              .SetData(CUBE_VERTICES.data(), verticesSize)
                              .Build();

    const uint64_t indicesSize{ sizeof(uint32_t) * model->indexCount };
    model->indexBuffer = prev::render::buffer::BufferBuilder{ device, device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                             .SetUsageFlags(GFX_BUFFER_USAGE_INDEX | GFX_BUFFER_USAGE_COPY_DST)
                             .SetMemoryProperties(GFX_MEMORY_PROPERTY_DEVICE_LOCAL)
                             .SetSize(indicesSize)
                             .SetData(indices.data(), indicesSize)
                             .Build();

    return model;
}
} // namespace sandbox::render
