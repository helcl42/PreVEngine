#include "FontMetadata.h"

namespace prev_test::render::font {
FontMetadata::FontMetadata(const std::shared_ptr<prev::render::buffer::ImageBuffer>& imageBuffer, const std::map<int, Character>& characterMetaData, const float spaceWidth, const float lineHeight)
    : m_imageBuffer(imageBuffer)
    , m_characterMetaData(characterMetaData)
    , m_spaceWidth(spaceWidth)
    , m_lineHeight(lineHeight)
{
}

float FontMetadata::GetSpaceWidth(const float fontSize) const
{
    return m_spaceWidth * fontSize;
}

float FontMetadata::GetLineHeight(const float fontSize) const
{
    return m_lineHeight * fontSize;
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
