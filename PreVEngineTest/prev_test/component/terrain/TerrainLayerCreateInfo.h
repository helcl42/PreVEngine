#ifndef __TERRAIN_LAYER_CREATE_INFO_H__
#define __TERRAIN_LAYER_CREATE_INFO_H__

#include <string>

namespace prev_test::component::terrain {
struct TerrainLayerCreateInfo {
    TerrainLayerCreateInfo(const std::string& matPath, const float shineDamperr, const float reflectivityy, const float step);

    TerrainLayerCreateInfo(const std::string& matPath, const std::string& normalPath, const float shineDamperr, const float reflectivityy, const float step);

    TerrainLayerCreateInfo(const std::string& matPath, const std::string& normalPath, const std::string& heightPath, const float shineDamperr, const float reflectivityy, const float heightScl, const float step);

    std::string materialPath;
    std::string materialNormalPath;
    std::string materialHeightPath;
    float shineDamper;
    float reflectivity;
    float heightScale;
    float heightStep;
};
} // namespace prev_test::component::terrain

#endif // !__TERRAIN_LAYER_CREATE_INFO_H__
