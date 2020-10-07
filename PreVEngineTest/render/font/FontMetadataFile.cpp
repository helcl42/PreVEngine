#include "FontMetadataFile.h"

#include <prev/util/Utils.h>

namespace prev_test::render::font {
FontMetadataFile::FontMetadataFile(const std::string& path)
{
    std::ifstream fileStram{ path };
    if (!fileStram.is_open()) {
        throw std::runtime_error("Could not open metadata file: " + path);
    }

    m_allLinesKeyValues = GetAllLinesTokens(fileStram);
    if (m_allLinesKeyValues.size() > 0) {
        m_currentLine = m_allLinesKeyValues.at(0);
    }
}

void FontMetadataFile::MoveToNextLine()
{
    m_currentLine = m_allLinesKeyValues.at(m_lineNumber++);
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
    auto numberStrings = prev::util::StringUtils::Split(m_currentLine.at(variable), ',');
    for (size_t i = 0; i < numberStrings.size(); i++) {
        actualValues.push_back(std::stoi(numberStrings[i]));
    }
    return actualValues;
}

bool FontMetadataFile::ProcessNextLine(std::ifstream& inOutFileStream, std::map<std::string, std::string>& outTokens) const
{
    outTokens.clear();

    std::string line;
    std::getline(inOutFileStream, line);

    if (line.empty()) {
        return false;
    }

    auto items = prev::util::StringUtils::Split(line, ' ');
    for (auto& part : items) {
        auto valuePairs = prev::util::StringUtils::Split(part, '=');
        if (valuePairs.size() == 2) {
            outTokens.insert({ valuePairs[0], valuePairs[1] });
        }
    }
    return true;
}

std::vector<std::map<std::string, std::string> > FontMetadataFile::GetAllLinesTokens(std::ifstream& inOutFileStream) const
{
    std::vector<std::map<std::string, std::string> > result{};

    std::map<std::string, std::string> lineTokens{};
    while (ProcessNextLine(inOutFileStream, lineTokens)) {
        result.emplace_back(lineTokens);
    }

    return result;
}

} // namespace prev_test::render::font
