#include "ShaderFactory.h"

#if defined(__ANDROID__)
#include <android_native.h>
#endif
#include <fstream>

namespace prev::render::shader {
std::vector<char> ShaderFactory::LoadByteCodeFromFile(const std::string& filename) const
{
    LOGI("Load Shader: %s\n", filename.c_str());

#if defined(__ANDROID__)
    FILE* file = fopen(filename.c_str(), "rb");
    if(!file) {
        throw std::runtime_error("Could not open shader file: " + filename);
    }
    stdiobuf sbuf(file);
    std::istream inStream(&sbuf);
#else
    std::ifstream inStream(filename, std::ios_base::binary);
#endif
    if (!inStream.good()) {
        throw std::runtime_error("Could not open shader file: " + filename);
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(inStream)), (std::istreambuf_iterator<char>()));

    return buffer;
}
} // namespace prev::render::shader