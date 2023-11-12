#include "VertexBoneData.h"

#include <prev/util/Utils.h>

#include <climits>

namespace prev_test::render::mesh {
VertexBoneData::VertexBoneData()
{
    Reset();
}

void VertexBoneData::Reset()
{
    std::memset(ids, 0, sizeof(ids));
    std::memset(weights, 0, sizeof(weights));
}

void VertexBoneData::AddBoneData(const unsigned int boneId, const float weight)
{
    unsigned int minWeightIndex{ 0 };
    float minWeight = std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < prev::util::ArraySize(ids); ++i) {
        if (weights[i] < minWeight) {
            minWeight = weights[i];
            minWeightIndex = i;
        }
    }

    if (minWeight < weight) {
        ids[minWeightIndex] = boneId;
        weights[minWeightIndex] = weight;
    }
}

void VertexBoneData::Normalize()
{
    float weightSum{ 0.0f };
    for (unsigned int i = 0; i < prev::util::ArraySize(ids); ++i) {
        weightSum += weights[i];
    }

    for (unsigned int i = 0; i < prev::util::ArraySize(ids); ++i) {
        weights[i] /= weightSum;
    }
}
} // namespace prev_test::render::mesh