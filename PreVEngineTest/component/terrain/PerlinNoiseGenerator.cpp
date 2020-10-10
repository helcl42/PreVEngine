#include "PerlinNoiseGenerator.h"

#include <random>

namespace prev_test::component::terrain {
PerlinNoiseGenerator::PerlinNoiseGenerator()
    : m_seed(GenerateSeed())
{
}

PerlinNoiseGenerator::PerlinNoiseGenerator(const unsigned int seed)
    : m_seed(seed)
{
}

float PerlinNoiseGenerator::GetInterpolatedNoise(const float x, const float z) const
{
    int intX = static_cast<int>(x);
    int intZ = static_cast<int>(z);
    float fracX = x - intX;
    float fracZ = z - intZ;

    float v1 = GetSmoothNoise(intX, intZ);
    float v2 = GetSmoothNoise(intX + 1, intZ);
    float v3 = GetSmoothNoise(intX, intZ + 1);
    float v4 = GetSmoothNoise(intX + 1, intZ + 1);

    float i1 = Interpolate(v1, v2, fracX);
    float i2 = Interpolate(v3, v4, fracX);
    return Interpolate(i1, i2, fracZ);
}

unsigned int PerlinNoiseGenerator::GenerateSeed()
{
    std::random_device r;
    std::default_random_engine eng{ r() };
    std::uniform_real_distribution<float> urd(0, 1000000000);
    return static_cast<unsigned int>(urd(eng));
}

float PerlinNoiseGenerator::Interpolate(const float a, const float b, const float blend)
{
    const float theta = blend * glm::pi<float>();
    const float factor = (1.0f - cosf(theta)) * 0.5f;
    return a * (1.0f - factor) + b * factor;
}

float PerlinNoiseGenerator::GetNoise(const int x, const int z) const
{
    const unsigned int seed = x * 49632 + z * 325176 + m_seed;
    std::default_random_engine eng{ static_cast<long unsigned int>(seed) };
    std::uniform_real_distribution<float> urd(-1.0, 1.0);
    return urd(eng);
}

float PerlinNoiseGenerator::GetSmoothNoise(const int x, const int z) const
{
    const float corners = (GetNoise(x - 1, z - 1) + GetNoise(x + 1, z - 1) + GetNoise(x - 1, z + 1) + GetNoise(x + 1, z + 1)) / 16.0f;
    const float sides = (GetNoise(x - 1, z) + GetNoise(x + 1, z) + GetNoise(x, z - 1) + GetNoise(x, z + 1)) / 8.0f;
    const float center = GetNoise(x, z) / 4.0f;
    return corners + sides + center;
}
} // namespace prev_test::component::terrain