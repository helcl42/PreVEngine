#include "MaterialFactory.h"

#include "Material.h"

namespace prev_test::render::material {
std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const ImagePair& image) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps, image);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps, image, normalMap);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const ImagePair& image, const ImagePair& normalMap, const ImagePair& heightMap) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps, image, normalMap, heightMap);
}
} // namespace prev_test::render::material