#ifndef __HEIGHT_GENERATOR_H__
#define __HEIGHT_GENERATOR_H__

#include "PerlinNoiseGenerator.h"

namespace prev_test::component::terrain {
class HeightGenerator {
public:
    explicit HeightGenerator();

    explicit HeightGenerator(const int x, const int z, const int size, const unsigned int seed);

    ~HeightGenerator() = default;

public:
    float GenerateHeight(const int x, const int z) const;

private:
    inline static const float AMPLITUDE{ 60.0f };

    inline static const int OCTAVES{ 3 };

    inline static const float ROUGHNESS{ 0.1f };

private:
    const int m_xOffset;

    const int m_zOffset;

    const std::unique_ptr<PerlinNoiseGenerator> m_noiseGenerator;
};
} // namespace prev_test::component::terrain

#endif // !__HEIGHT_GENERATOR_H__
