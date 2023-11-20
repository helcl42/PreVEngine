#include "Utils.h"

#if defined(__ANDROID__)
#include <android_native.h>
#endif

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace prev::util {

namespace file {
    bool CreateDirectoryByPath(const std::string& path)
    {
        return std::filesystem::create_directories(path);
    }

    std::string GetDirectoryPath(const std::string& filePath)
    {
        std::filesystem::path p(filePath);
        std::filesystem::path parent = p.parent_path();
        return parent.string();
    }

#if defined(__ANDROID__)
    bool Exists(const std::string& filePath)
    {
        AAsset* file = android_open_asset(filePath.c_str(), AASSET_MODE_STREAMING);
        if (!file) {
            return false;
        }
        AAsset_close(file);
        return true;
    }

    std::string ReadTextFile(const std::string& filePath)
    {
        AAsset* file = android_open_asset(filePath.c_str(), AASSET_MODE_BUFFER);
        if (!file) {
            throw std::runtime_error("Could not open file: " + filePath);
        }
        size_t fileLength = AAsset_getLength(file);
        std::string text;
        text.resize(fileLength);
        AAsset_read(file, (void*)text.data(), fileLength);
        AAsset_close(file);
        return text;
    }

    std::vector<char> ReadBinaryFile(const std::string& filePath)
    {
        AAsset* file = android_open_asset(filePath.c_str(), AASSET_MODE_BUFFER);
        size_t fileLength = AAsset_getLength(file);
        std::vector<char> binary(fileLength);
        AAsset_read(file, (void*)binary.data(), fileLength);
        AAsset_close(file);
        return binary;
    }
#else
    bool Exists(const std::string& filePath)
    {
        return std::filesystem::exists(filePath);
    }

    std::string ReadTextFile(const std::string& filePath)
    {
        std::ifstream stream(filePath, std::fstream::in);
        if (!stream.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }
        std::string output;
        std::string line;
        while (!stream.eof()) {
            std::getline(stream, line);
            output.append(line + "\n");
        }
        stream.close();
        return output;
    }

    std::vector<char> ReadBinaryFile(const std::string& filePath)
    {
        std::ifstream stream(filePath, std::fstream::in | std::fstream::binary | std::fstream::ate);
        if (!stream.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }
        std::streamoff size = stream.tellg();
        std::vector<char> output(static_cast<size_t>(size));
        stream.seekg(0, std::fstream::beg);
        stream.read(output.data(), size);
        stream.close();
        return output;
    }
#endif
} // namespace file

namespace string {
    std::vector<std::string> Split(const std::string& s, const char delim)
    {
        std::vector<std::string> elems;
        std::istringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::wstring> Split(const std::wstring& s, const wchar_t delim)
    {
        std::vector<std::wstring> elems;
        std::wstringstream ss(s);
        std::wstring item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> Split(const std::string& s, const std::string& t)
    {
        std::string copy{ s };
        std::vector<std::string> res;
        while (true) {
            size_t pos = copy.find(t);
            if (pos == std::string::npos) {
                res.push_back(copy);
                break;
            }
            res.push_back(copy.substr(0, pos));
            copy = copy.substr(pos + 1, copy.size() - pos - 1);
        }
        return res;
    }

    std::vector<std::wstring> Split(const std::wstring& s, const std::wstring& t)
    {
        std::wstring copy{ s };
        std::vector<std::wstring> res;
        while (true) {
            size_t pos = copy.find(t);
            if (pos == std::string::npos) {
                res.push_back(copy);
                break;
            }
            res.push_back(copy.substr(0, pos));
            copy = copy.substr(pos + 1, copy.size() - pos - 1);
        }
        return res;
    }

    std::string Replace(const std::string& subject, const std::string& search, const std::string& replace)
    {
        std::string copy{ subject };
        size_t pos{ 0 };
        while ((pos = copy.find(search, pos)) != std::string::npos) {
            copy.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return copy;
    }

    std::wstring Replace(const std::wstring& subject, const std::wstring& search, const std::wstring& replace)
    {
        std::wstring copy{ subject };
        size_t pos{ 0 };
        while ((pos = copy.find(search, pos)) != std::wstring::npos) {
            copy.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return copy;
    }

    std::string GetAsString(const glm::vec2& v, const uint32_t precission)
    {
        std::stringstream ss;
        ss << std::setprecision(precission) << "(" << v.x << ", " << v.y << ")";
        return ss.str();
    }

    std::string GetAsString(const glm::vec3& v, const uint32_t precission)
    {
        std::stringstream ss;
        ss << std::setprecision(precission) << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return ss.str();
    }

    std::string GetAsString(const glm::vec4& v, const uint32_t precission)
    {
        std::stringstream ss;
        ss << std::setprecision(precission) << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        return ss.str();
    }

    std::string GetAsString(const glm::quat& q, const uint32_t precission)
    {
        std::stringstream ss;
        ss << std::setprecision(precission) << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
        return ss.str();
    }
} // namespace string

} // namespace prev::util