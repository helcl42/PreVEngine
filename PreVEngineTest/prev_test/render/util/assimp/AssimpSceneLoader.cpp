#include "AssimpSceneLoader.h"

#include <prev/util/Utils.h>

namespace prev_test::render::util::assimp {
bool AssimpSceneLoader::LoadScene(const std::string& modelPath, Assimp::Importer& importer, const aiScene** scene) const
{
    const unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals | aiProcess_FindInvalidData | aiProcess_ForceGenNormals | aiProcess_ValidateDataStructure;

    const std::vector<char> fileData{ prev::util::file::ReadBinaryFile(modelPath) };
    *scene = importer.ReadFileFromMemory(fileData.data(), fileData.size(), flags);
    if (!*scene) {
        return false;
    }
    return true;
}
} // namespace prev_test::render::util::assimp