#include "AssimpSceneLoader.h"

namespace prev_test::render::util::assimp {
bool AssimpSceneLoader::LoadScene(const std::string& modelPath, Assimp::Importer* importer, const aiScene** scene) const
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
} // namespace prev_test::render::util::assimp