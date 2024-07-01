#include "FontRenderComponentsFactory.h"
#include "FontRenderComponent.h"

#include "../../render/font/FontMetadataFactory.h"

namespace prev_test::component::font {
std::unique_ptr<prev_test::render::font::FontMetadata> FontRenderComponentsFactory::CreateFontMetadata(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const
{
    return prev_test::render::font::FontMetadataFactory{}.CreateFontMetadata(fontPath, fontTexture, aspectRatio, padding);
}
} // namespace prev_test::component::font