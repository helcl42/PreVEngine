#ifndef __MATERIAL_FACTORY_H__
#define __MATERIAL_FACTORY_H__

#include "../IMaterial.h"

namespace prev_test::render::material {
class MaterialFactory final {
public:
    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const ImagePair& image) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap) const;

    std::unique_ptr<prev_test::render::IMaterial> Create(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap, const ImagePair& heightMap) const;

    std::vector<std::unique_ptr<prev_test::render::IMaterial> > Create(const std::string& modelPath) const;
};
} // namespace prev_test::render::material

#endif // !__MATERIAL_FACTORY_H__
