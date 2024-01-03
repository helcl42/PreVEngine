#include "FontRenderComponentsFactory.h"
#include "DefaultFontRenderComponent.h"

#include "../../render/font/FontMetadataFactory.h"

namespace prev_test::component::font {
std::unique_ptr<IFontRenderComponent> FontRenderComponentsFactory::Create(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const
{
    prev_test::render::font::FontMetadataFactory fontFactory{};
    auto fontMetaData = fontFactory.CreateFontMetadata(fontPath, fontTexture, aspectRatio, padding);

    return std::make_unique<DefaultFontRenderComponent>(std::move(fontMetaData));
}
} // namespace prev_test::component::font