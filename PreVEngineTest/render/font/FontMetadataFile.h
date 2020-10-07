#ifndef __FONT_METADATA_FILE_H__
#define __FONT_METADATA_FILE_H__

#include <map>
#include <string>
#include <vector>
#include <fstream>

namespace prev_test::render::font {
class FontMetadataFile {
public:
    FontMetadataFile(const std::string& path);

    ~FontMetadataFile() = default;

public:
    void MoveToNextLine();

    bool ValuesContains(const std::string& variable) const;

    std::string GetValueAsString(const std::string& variable) const;

    int GetValueAsInt(const std::string& variable) const;

    std::vector<int> GetValueAsInts(const std::string& variable) const;

private:
    bool ProcessNextLine(std::ifstream& inOutFileStream, std::map<std::string, std::string>& outTokens) const;

    std::vector<std::map<std::string, std::string> > GetAllLinesTokens(std::ifstream& inOutFileStream) const;

private:
    std::vector<std::map<std::string, std::string> > m_allLinesKeyValues;

    std::map<std::string, std::string> m_currentLine;

    uint32_t m_lineNumber{ 0 };
};
} // namespace prev_test::render::font

#endif // !__FONT_METADATA_FILE_H__
