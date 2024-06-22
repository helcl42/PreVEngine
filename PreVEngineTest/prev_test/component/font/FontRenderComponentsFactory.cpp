#include "FontRenderComponentsFactory.h"
#include "FontRenderComponent.h"

#include "../../render/font/FontMetadataFactory.h"

namespace prev_test::component::font {
std::unique_ptr<IFontRenderComponent> FontRenderComponentsFactory::Create2d(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const
{
    prev_test::render::font::FontMetadataFactory fontFactory{};
    auto fontMetaData = fontFactory.CreateFontMetadata(fontPath, fontTexture, aspectRatio, padding);

    return std::make_unique<FontRenderComponent>(std::move(fontMetaData));
}

std::unique_ptr<IFont3dRenderComponent> FontRenderComponentsFactory::Create3d(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const
{
    prev_test::render::font::FontMetadataFactory fontFactory{};
    auto fontMetaData = fontFactory.CreateFontMetadata(fontPath, fontTexture, aspectRatio, padding);

    return std::make_unique<Font3dRenderComponent>(std::move(fontMetaData));
}
} // namespace prev_test::component::font