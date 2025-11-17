#ifndef __VERTEX_LAYOUT_H__
#define __VERTEX_LAYOUT_H__

#include "VertexLayoutComponent.h"

#include <prev/common/Common.h>

#include <vector>

namespace prev_test::render {
struct VertexLayout final {
public:
    VertexLayout() = default;

    VertexLayout(const std::vector<VertexLayoutComponent>& components);

public:
    const std::vector<VertexLayoutComponent>& GetComponents() const;

    uint32_t GetStride() const;

public:
    static uint32_t GetComponentSize(const VertexLayoutComponent component);

    static uint32_t GetComponentsSize(const std::vector<VertexLayoutComponent>& components);

private:
    std::vector<VertexLayoutComponent> m_components;
};
} // namespace prev_test::render

#endif // !__VERTEX_LAYOUT_H__
