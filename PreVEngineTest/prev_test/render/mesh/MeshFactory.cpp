#include "MeshFactory.h"

#include "CubeMesh.h"
#include "PlaneMesh.h"
#include "QuadMesh.h"
#include "SphereMesh.h"

namespace prev_test::render::mesh {
std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreatePlane(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float textureCoordUMax, const float textureCoordVMax, const FlatMeshConstellation constellation, const bool generateTangentBiTangent) const
{
    return std::make_unique<PlaneMesh>(xSize, zSize, xDivs, zDivs, textureCoordUMax, textureCoordVMax, constellation, generateTangentBiTangent);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateCube(const bool generateTangentBiTangent) const
{
    return std::make_unique<CubeMesh>(generateTangentBiTangent);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateSphere(const float radius, const int subDivY, const int subDivZ, const float degreesHorizontal, const float degreesVertical, const bool generateTangentBiTangent) const
{
    return std::make_unique<SphereMesh>(radius, subDivY, subDivZ, degreesHorizontal, degreesVertical, generateTangentBiTangent);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateQuad(const FlatMeshConstellation constellation, const bool generateTangentBiTangent) const
{
    return std::make_unique<QuadMesh>(constellation, generateTangentBiTangent);
}
} // namespace prev_test::render::mesh