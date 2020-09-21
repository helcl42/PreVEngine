#ifndef __VERTEX_LAYOUT_COMPONENT_H__
#define __VERTEX_LAYOUT_COMPONENT_H__

namespace prev_test::render {
enum class VertexLayoutComponent {
    FLOAT = 0x0,
    VEC2 = 0x1,
    VEC3 = 0x2,
    VEC4 = 0x3,
    IVEC = 0x4,
    IVEC2 = 0x5,
    IVEC3 = 0x6,
    IVEC4 = 0x7,
    MAT3 = 0x8,
    MAT4 = 0x9
};
}

#endif // !__VERTEX_LAYOUT_COMPONENT_H__
