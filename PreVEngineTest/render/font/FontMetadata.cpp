#include "FontMetadata.h"

namespace prev_test::render::font {
float FontMetadata::GetSpaceWidth() const
{
    return m_spaceWidth;
}

float FontMetadata::GetFontSizeScaledSpaceWidth(const float fontSize)
{
    return m_spaceWidth * fontSize;
}

std::shared_ptr<prev::render::image::Image> FontMetadata::GetImage() const
{
    return m_image;
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> FontMetadata::GetImageBuffer() const
{
    return m_imageBuffer;
}

bool FontMetadata::GetCharacter(const int charCode, Character& outCharacter) const
{
    if (m_characterMetaData.find(charCode) != m_characterMetaData.cend()) {
        outCharacter = m_characterMetaData.at(charCode);
        return true;
    }
    return false;
}
} // namespace prev_test::render::font
