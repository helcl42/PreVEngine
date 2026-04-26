#include "FontRenderComponentsFactory.h"
#include "FontRenderComponent.h"

#include "../../render/font/FontMetadataFactory.h"

namespace prev_test::component::font {
FontRenderComponentsFactory::FontRenderComponentsFactory(prev::core::device::Device& device)
    : m_device{ device }
{
}

std::unique_ptr<prev_test::render::font::FontMetadata> FontRenderComponentsFactory::CreateFontMetadata(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const float lineHeight, const int padding) const
{
    return prev_test::render::font::FontMetadataFactory{ m_device }.CreateFontMetadata(fontPath, fontTexture, aspectRatio, lineHeight, padding);
}
} // namespace prev_test::component::font