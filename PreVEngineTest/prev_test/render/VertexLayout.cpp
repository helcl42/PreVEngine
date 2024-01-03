#include "VertexLayout.h"

#include <stdexcept>

namespace prev_test::render {
VertexLayout::VertexLayout(const std::vector<VertexLayoutComponent>& components)
    : m_components(components)
{
}

const std::vector<VertexLayoutComponent>& VertexLayout::GetComponents() const
{
    return m_components;
}

uint32_t VertexLayout::GetStride() const
{
    return VertexLayout::GetComponentsSize(m_components);
}

uint32_t VertexLayout::GetComponentSize(const VertexLayoutComponent component)
{
    switch (component) {
    case VertexLayoutComponent::FLOAT:
        return 1 * sizeof(float);
    case VertexLayoutComponent::VEC2:
        return 2 * sizeof(float);
    case VertexLayoutComponent::VEC3:
        return 3 * sizeof(float);
    case VertexLayoutComponent::VEC4:
        return 4 * sizeof(float);
    case VertexLayoutComponent::IVEC:
        return 1 * sizeof(int32_t);
    case VertexLayoutComponent::IVEC2:
        return 2 * sizeof(int32_t);
    case VertexLayoutComponent::IVEC3:
        return 3 * sizeof(int32_t);
    case VertexLayoutComponent::IVEC4:
        return 4 * sizeof(int32_t);
    case VertexLayoutComponent::MAT3:
        return 3 * 3 * sizeof(float);
    case VertexLayoutComponent::MAT4:
        return 4 * 4 * sizeof(float);
    default:
        throw std::runtime_error("Invalid vertex layout component type.");
    }
}

uint32_t VertexLayout::GetComponentsSize(const std::vector<VertexLayoutComponent>& components)
{
    uint32_t singleVertexPackSizeInBytes = 0;
    for (const auto& component : components) {
        singleVertexPackSizeInBytes += VertexLayout::GetComponentSize(component);
    }
    return singleVertexPackSizeInBytes;
}
} // namespace prev_test::render