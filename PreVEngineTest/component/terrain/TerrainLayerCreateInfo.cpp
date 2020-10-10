#include "TerrainLayerCreateInfo.h"

namespace prev_test::component::terrain {
TerrainLayerCreateInfo::TerrainLayerCreateInfo(const std::string& matPath, const float shineDamperr, const float reflectivityy, const float step)
    : materialPath(matPath)
    , shineDamper(shineDamperr)
    , reflectivity(reflectivityy)
    , heightStep(step)
{
}

TerrainLayerCreateInfo::TerrainLayerCreateInfo(const std::string& matPath, const std::string& normalPath, const float shineDamperr, const float reflectivityy, const float step)
    : materialPath(matPath)
    , materialNormalPath(normalPath)
    , shineDamper(shineDamperr)
    , reflectivity(reflectivityy)
    , heightStep(step)
{
}

TerrainLayerCreateInfo::TerrainLayerCreateInfo(const std::string& matPath, const std::string& normalPath, const std::string& heightPath, const float shineDamperr, const float reflectivityy, const float heightScl, const float step)
    : materialPath(matPath)
    , materialNormalPath(normalPath)
    , materialHeightPath(heightPath)
    , shineDamper(shineDamperr)
    , reflectivity(reflectivityy)
    , heightScale(heightScl)
    , heightStep(step)
{
}
} // namespace prev_test::component::terrain