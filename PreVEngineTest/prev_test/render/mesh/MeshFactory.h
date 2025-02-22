#ifndef __MESH_FACTORY_H__
#define __MESH_FACTORY_H__

#include "../IMesh.h"

#include <memory>

namespace prev_test::render::mesh {

class MeshFactory final {
public:
    std::unique_ptr<prev_test::render::IMesh> CreatePlane(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float textureCoordUMax = 1.0f, const float textureCoordVMax = 1.0f, const FlatMeshConstellation constellation = FlatMeshConstellation::ZERO_Y, const bool generateTangentBiTangent = false) const;

    std::unique_ptr<prev_test::render::IMesh> CreateCube(const bool generateTangentBiTangent = false) const;

    std::unique_ptr<prev_test::render::IMesh> CreateSphere(const float radius, const int subDivY, const int subDivZ, const float degreesHorizontal = 360.0f, const float degreesVertical = 180.0f, const bool generateTangentBiTangent = false) const;

    std::unique_ptr<prev_test::render::IMesh> CreateQuad(const FlatMeshConstellation constellation = FlatMeshConstellation::ZERO_Z, const bool generateTangentBiTangent = false) const;
};
} // namespace prev_test::render::mesh

#endif