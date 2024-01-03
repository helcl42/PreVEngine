#ifndef __MESH_UTIL_H__
#define __MESH_UTIL_H__

#include <prev/common/Common.h>

#include "../IMesh.h"

#include <memory>
#include <vector>

namespace prev_test::render::mesh {
class MeshUtil {
public:
    static std::vector<glm::vec3> GenerateNormals(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices, const bool smooth);

    static std::tuple<std::vector<glm::vec3>, std::vector<glm::vec3>> GenerateTangetsAndBiTangents(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<glm::vec3>& normals, const std::vector<uint32_t>& indices);

    static std::vector<glm::vec3> GetMeshTransformedVertices(const std::shared_ptr<prev_test::render::IMesh>& mesh);
};
} // namespace prev_test::render::mesh

#endif // !__MESH_UTIL_H__
