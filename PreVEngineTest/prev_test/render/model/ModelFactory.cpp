#include "ModelFactory.h"

#include "Model.h"

#include <prev/render/buffer/BufferBuilder.h>

namespace prev_test::render::model {
ModelFactory::ModelFactory(prev::core::memory::Allocator& allocator)
    : m_allocator{ allocator }
{
}

std::unique_ptr<prev_test::render::IModel> ModelFactory::Create(const std::shared_ptr<IMesh>& mesh) const
{
    auto vertexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                            .SetMemoryType(prev::core::memory::MemoryType::DEVICE_LOCAL)
                            .SetUsageFlags(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
                            .SetSize(mesh->GetVertexLayout().GetStride() * mesh->GerVerticesCount())
                            .SetData(mesh->GetVertexData())
                            .Build();

    auto indexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::DEVICE_LOCAL)
                           .SetUsageFlags(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                           .SetSize(sizeof(uint32_t) * mesh->GetIndicesCount())
                           .SetData(mesh->GetIndices().data())
                           .Build();

    return std::make_unique<prev_test::render::model::Model>(mesh, std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> ModelFactory::Create(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::render::buffer::Buffer>& vertexBuffer, const std::shared_ptr<prev::render::buffer::Buffer>& indexBuffer) const
{
    return std::make_unique<prev_test::render::model::Model>(mesh, vertexBuffer, indexBuffer);
}

std::unique_ptr<prev_test::render::IModel> ModelFactory::CreateHostVisible(const std::shared_ptr<IMesh>& mesh, const uint32_t maxVertexCount, const uint32_t maxIndexCount) const
{
    const uint32_t finalVertexCount{ std::max(mesh->GerVerticesCount(), maxVertexCount) };
    const uint32_t finalIndexCount{ std::max(static_cast<uint32_t>(mesh->GetIndices().size()), maxIndexCount) };

    auto vertexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                            .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                            .SetUsageFlags(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
                            .SetSize(mesh->GetVertexLayout().GetStride() * finalVertexCount)
                            .SetData(mesh->GetVertexData())
                            .Build();

    auto indexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                           .SetUsageFlags(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                           .SetSize(sizeof(uint32_t) * finalIndexCount)
                           .SetData(mesh->GetIndices().data())
                           .Build();

    return std::make_unique<prev_test::render::model::Model>(mesh, std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::render::model