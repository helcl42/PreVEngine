#ifndef __VERTEX_BONE_DATA_H__
#define __VERTEX_BONE_DATA_H__

namespace prev_test::render::mesh {
struct VertexBoneData {
    inline static const int BONES_PER_VERTEX_COUNT{ 4 };

    unsigned int ids[BONES_PER_VERTEX_COUNT];

    float weights[BONES_PER_VERTEX_COUNT];

    VertexBoneData();

    ~VertexBoneData() = default;

    void Reset();

    void AddBoneData(const unsigned int boneId, const float weight);
};
} // namespace prev_test::render::mesh

#endif // !__VERTEX_BONE_DATA_H__
