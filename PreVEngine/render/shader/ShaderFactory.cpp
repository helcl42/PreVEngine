#include "ShaderFactory.h"

#include <fstream>

namespace PreVEngine {
std::vector<char> ShaderFactory::LoadByteCodeFromFile(const std::string& filename) const
{
    LOGI("Load Shader: %s\n", filename.c_str());

    std::ifstream fileStream(filename, std::ios_base::binary);

    if (!fileStream.good()) {
        throw std::runtime_error("Could not open shader file: " + filename);
    }

    fileStream.seekg(0, fileStream.end);
    size_t length = fileStream.tellg();
    fileStream.seekg(0, fileStream.beg);

    assert(length > 0 && "Could not read file content");

    std::vector<char> buffer;

    buffer.resize(length);
    fileStream.read(&buffer[0], length);

    fileStream.close();

    return buffer;
}
} // namespace PreVEngine