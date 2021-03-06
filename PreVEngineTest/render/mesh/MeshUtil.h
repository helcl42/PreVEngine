#ifndef __MESH_UTIL_H__
#define __MESH_UTIL_H__

#include <prev/common/Common.h>

#include "../IMesh.h"

#include <vector>

namespace prev_test::render::mesh {
class MeshUtil {
public:
    static void GenerateTangetsAndBiTangents(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices, std::vector<glm::vec3>& outTangents, std::vector<glm::vec3>& outBiTangents);

    static std::vector<glm::vec3> GetMeshTransformedVertices(const std::shared_ptr<prev_test::render::IMesh>& mesh);
};
} // namespace prev_test::render::mesh

#endif // !__MESH_UTIL_H__
