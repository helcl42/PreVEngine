#include "FontMetadataFile.h"

#include <prev/util/Utils.h>

namespace prev_test::render::font {
FontMetadataFile::FontMetadataFile(const std::string& path)
{
    const std::string allText{ prev::util::file::ReadTextFile(path) };
    const std::vector<std::string> allLines{ prev::util::string::Split(allText, '\n') };

    m_allLinesKeyValues = GetAllLinesTokens(allLines);
    if (m_allLinesKeyValues.size() > 0) {
        m_currentLine = m_allLinesKeyValues[0];
    }
}

void FontMetadataFile::MoveToNextLine()
{
    m_currentLine = m_allLinesKeyValues[m_lineNumber++];
}

bool FontMetadataFile::ValuesContains(const std::string& variable) const
{
    return m_currentLine.find(variable) != m_currentLine.cend();
}

std::string FontMetadataFile::GetValueAsString(const std::string& variable) const
{
    return m_currentLine.at(variable);
}

int FontMetadataFile::GetValueAsInt(const std::string& variable) const
{
    return std::stoi(m_currentLine.at(variable));
}

std::vector<int> FontMetadataFile::GetValueAsInts(const std::string& variable) const
{
    std::vector<int> actualValues;
    auto numberStrings = prev::util::string::Split(m_currentLine.at(variable), ',');
    for (size_t i = 0; i < numberStrings.size(); ++i) {
        actualValues.push_back(std::stoi(numberStrings[i]));
    }
    return actualValues;
}

std::map<std::string, std::string> FontMetadataFile::GetLineTokens(const std::string& line) const
{
    std::map<std::string, std::string> tokens;
    auto items = prev::util::string::Split(line, ' ');
    for (auto& part : items) {
        auto valuePairs = prev::util::string::Split(part, '=');
        if (valuePairs.size() == 2) {
            tokens.insert({ valuePairs[0], valuePairs[1] });
        }
    }
    return tokens;
}

std::vector<std::map<std::string, std::string>> FontMetadataFile::GetAllLinesTokens(const std::vector<std::string>& lines) const
{
    std::vector<std::map<std::string, std::string>> allLinesTokens{};
    for (const auto& line : lines) {
        if (!line.empty()) {
            const auto lineTokens{ GetLineTokens(line) };
            allLinesTokens.push_back(lineTokens);
        }
    }
    return allLinesTokens;
}

} // namespace prev_test::render::font
