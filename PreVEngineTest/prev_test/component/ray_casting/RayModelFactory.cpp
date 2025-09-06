#include "RayModelFactory.h"

#include "../../render/model/Model.h"

#include <prev/render/buffer/BufferBuilder.h>

namespace prev_test::component::ray_casting {
RayModelFactory::RayModelFactory(prev::core::memory::Allocator& allocator)
    : m_allocator{ allocator }
{
}

std::unique_ptr<prev_test::render::IModel> RayModelFactory::Create(const prev_test::common::intersection::Ray& ray) const
{
    const float DISTANCE_BETWEEN_POINTS{ 4.0f };
    const uint32_t pointsCount{ static_cast<uint32_t>(ray.length / DISTANCE_BETWEEN_POINTS) };

    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    for (uint32_t i = 1; i < pointsCount; ++i) {
        vertices.emplace_back(ray.origin + ray.direction * (static_cast<float>(i) * DISTANCE_BETWEEN_POINTS));
        indices.emplace_back(i);
    }

    const auto verticesDataSize{ sizeof(glm::vec3) * vertices.size() };
    auto vertexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                            .SetMemoryType(prev::core::memory::MemoryType::DEVICE_LOCAL)
                            .SetUsageFlags(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
                            .SetSize(verticesDataSize)
                            .SetData(vertices.data(), verticesDataSize)
                            .Build();

    const auto indicesDataSize{ sizeof(uint32_t) * indices.size() };
    auto indexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::DEVICE_LOCAL)
                           .SetUsageFlags(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                           .SetSize(indicesDataSize)
                           .SetData(indices.data(), indicesDataSize)
                           .Build();

    return std::make_unique<prev_test::render::model::Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::component::ray_casting