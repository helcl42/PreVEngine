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

std::shared_ptr<prev::render::buffer::ImageBuffer> FontMetadata::GetImageBuffer() const
{
    return m_imageBuffer;
}

bool FontMetadata::GetCharacter(const int charCode, Character& outCharacter) const
{
    const auto charIter{ m_characterMetaData.find(charCode) };
    if (charIter != m_characterMetaData.cend()) {
        outCharacter = charIter->second;
        return true;
    }
    return false;
}
} // namespace prev_test::render::font
