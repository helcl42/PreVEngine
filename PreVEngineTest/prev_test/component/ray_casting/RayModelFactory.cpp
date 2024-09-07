#include "RayModelFactory.h"
#include "../../render/model/Model.h"

#include <prev/render/buffer/IndexBuffer.h>
#include <prev/render/buffer/VertexBuffer.h>

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

    auto vertexBuffer{ std::make_unique<prev::render::buffer::VertexBuffer>(m_allocator) };
    vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
    auto indexBuffer{ std::make_unique<prev::render::buffer::IndexBuffer>(m_allocator) };
    indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));

    return std::make_unique<prev_test::render::model::Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::component::ray_casting