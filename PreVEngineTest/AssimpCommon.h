#ifndef __ASSIMP_COMMON_H__
#define __ASSIMP_COMMON_H__

#include "General.h"

#include <Utils.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct VertexBoneData {
    inline static const int BONES_PER_VERTEX_COUNT = 4;

    unsigned int ids[BONES_PER_VERTEX_COUNT];

    float weights[BONES_PER_VERTEX_COUNT];

    VertexBoneData()
    {
        Reset();
    }

    ~VertexBoneData() = default;

    void Reset()
    {
        std::memset(ids, 0, sizeof(ids));
        std::memset(weights, 0, sizeof(weights));
    }

    void AddBoneData(const unsigned int boneId, const float weight)
    {
        unsigned int minWeightIndex = 0;
        float minWeight = std::numeric_limits<float>::max();
        for (unsigned int i = 0; i < ArraySize(ids); i++) {
            if (weights[i] < minWeight) {
                minWeight = weights[i];
                minWeightIndex = i;
            }
        }

        if (minWeight < weight) {
            ids[minWeightIndex] = boneId;
            weights[minWeightIndex] = weight;

            float weightSum = 0.0f;
            for (unsigned int i = 0; i < ArraySize(ids); i++) {
                weightSum += weights[i];
            }

            for (unsigned int i = 0; i < ArraySize(ids); i++) {
                weights[i] /= weightSum;
            }
        }
    }
};

class AssimpSceneLoader {
public:
    bool LoadScene(const std::string& modelPath, Assimp::Importer* importer, const aiScene** scene)
    {
        const unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals | aiProcess_FindInvalidData;

#if defined(__ANDROID__)
        AAsset* asset = android_open_asset(modelPath.c_str(), AASSET_MODE_STREAMING);
        assert(asset);
        size_t size = AAsset_getLength(asset);

        assert(size > 0);

        void* meshData = malloc(size);
        AAsset_read(asset, meshData, size);
        AAsset_close(asset);

        *scene = importer->ReadFileFromMemory(meshData, size, flags);

        free(meshData);
#else
        *scene = importer->ReadFile(modelPath, flags);
#endif
        if (!*scene) {
            return false;
        }
        return true;
    }
};


#endif // !__ASSIMP_COMMON_H__
