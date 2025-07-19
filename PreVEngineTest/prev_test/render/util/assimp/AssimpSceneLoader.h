#ifndef __ASSIMP_SCENE_LOADER_H__
#define __ASSIMP_SCENE_LOADER_H__

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace prev_test::render::util::assimp {
class AssimpSceneLoader final {
public:
    bool LoadScene(const std::string& modelPath, Assimp::Importer& importer, const aiScene** scene) const;
};
} // namespace prev_test::render::util::assimp

#endif // !__ASSIMP_SCENE_LOADER_H__
