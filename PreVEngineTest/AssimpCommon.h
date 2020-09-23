#ifndef __ASSIMP_COMMON_H__
#define __ASSIMP_COMMON_H__

#include "General.h"

#include <prev/util/Utils.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct VertexBoneData {
    inline static const int BONES_PER_VERTEX_COUNT{ 4 };

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
        for (unsigned int i = 0; i < prev::util::ArraySize(ids); i++) {
            if (weights[i] < minWeight) {
                minWeight = weights[i];
                minWeightIndex = i;
            }
        }

        if (minWeight < weight) {
            ids[minWeightIndex] = boneId;
            weights[minWeightIndex] = weight;

            float weightSum = 0.0f;
            for (unsigned int i = 0; i < prev::util::ArraySize(ids); i++) {
                weightSum += weights[i];
            }

            for (unsigned int i = 0; i < prev::util::ArraySize(ids); i++) {
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

class AssimpGlmConvertor {
public:
    static glm::mat4 ToGlmMat4(const aiMatrix3x3& inputMatrix)
    {
        glm::mat4 newMatrix;
        newMatrix[0][0] = inputMatrix.a1;
        newMatrix[1][0] = inputMatrix.a2;
        newMatrix[2][0] = inputMatrix.a3;
        newMatrix[3][0] = 0.0f;
        newMatrix[0][1] = inputMatrix.b1;
        newMatrix[1][1] = inputMatrix.b2;
        newMatrix[2][1] = inputMatrix.b3;
        newMatrix[3][1] = 0.0f;
        newMatrix[0][2] = inputMatrix.c1;
        newMatrix[1][2] = inputMatrix.c2;
        newMatrix[2][2] = inputMatrix.c3;
        newMatrix[3][2] = 0.0f;
        newMatrix[0][3] = 0.0f;
        newMatrix[1][3] = 0.0f;
        newMatrix[2][3] = 0.0f;
        newMatrix[3][3] = 1.0f;
        return newMatrix;
    }

    static glm::mat4 ToGlmMat4(const aiMatrix4x4& inputMatrix)
    {
        glm::mat4 newMatrix;
        newMatrix[0][0] = inputMatrix.a1;
        newMatrix[1][0] = inputMatrix.a2;
        newMatrix[2][0] = inputMatrix.a3;
        newMatrix[3][0] = inputMatrix.a4;
        newMatrix[0][1] = inputMatrix.b1;
        newMatrix[1][1] = inputMatrix.b2;
        newMatrix[2][1] = inputMatrix.b3;
        newMatrix[3][1] = inputMatrix.b4;
        newMatrix[0][2] = inputMatrix.c1;
        newMatrix[1][2] = inputMatrix.c2;
        newMatrix[2][2] = inputMatrix.c3;
        newMatrix[3][2] = inputMatrix.c4;
        newMatrix[0][3] = inputMatrix.d1;
        newMatrix[1][3] = inputMatrix.d2;
        newMatrix[2][3] = inputMatrix.d3;
        newMatrix[3][3] = inputMatrix.d4;
        return newMatrix;
    }

    static glm::vec3 ToGlmVec3(const aiVector3D& inputVector)
    {
        return glm::vec3(inputVector.x, inputVector.y, inputVector.z);
    }

    static glm::vec2 ToGlmVec2(const aiVector2D& inputVector)
    {
        return glm::vec2(inputVector.x, inputVector.y);
    }
};

#endif // !__ASSIMP_COMMON_H__
