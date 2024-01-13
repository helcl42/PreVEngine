#include "PerlinNoiseGenerator.h"

#include <prev/util/Utils.h>

namespace prev_test::component::terrain {
PerlinNoiseGenerator::PerlinNoiseGenerator()
    : m_seed{ GenerateSeed() }
{
}

PerlinNoiseGenerator::PerlinNoiseGenerator(const unsigned long seed)
    : m_seed{ seed }
{
}

float PerlinNoiseGenerator::GetInterpolatedNoise(const float x, const float z) const
{
    const int intX{ static_cast<int>(x) };
    const int intZ{ static_cast<int>(z) };
    const float fracX{ x - intX };
    const float fracZ{ z - intZ };

    const float v1{ GetSmoothNoise(intX, intZ) };
    const float v2{ GetSmoothNoise(intX + 1, intZ) };
    const float v3{ GetSmoothNoise(intX, intZ + 1) };
    const float v4{ GetSmoothNoise(intX + 1, intZ + 1) };

    const float i1{ Interpolate(v1, v2, fracX) };
    const float i2{ Interpolate(v3, v4, fracX) };
    return Interpolate(i1, i2, fracZ);
}

float PerlinNoiseGenerator::GetNoise(const int x, const int z) const
{
    // TODO -> use one generator per instance not per call ???
    const unsigned long Seed{ x * 49632 + z * 325176 + m_seed };
    prev::util::RandomNumberGenerator rng{ Seed };
    std::uniform_real_distribution<float> urd(-1.0, 1.0);
    return urd(rng.GetRandomEngine());
}

float PerlinNoiseGenerator::GetSmoothNoise(const int x, const int z) const
{
    const float corners{ (GetNoise(x - 1, z - 1) + GetNoise(x + 1, z - 1) + GetNoise(x - 1, z + 1) + GetNoise(x + 1, z + 1)) / 16.0f };
    const float sides{ (GetNoise(x - 1, z) + GetNoise(x + 1, z) + GetNoise(x, z - 1) + GetNoise(x, z + 1)) / 8.0f };
    const float center{ GetNoise(x, z) / 4.0f };
    return corners + sides + center;
}

unsigned long PerlinNoiseGenerator::GenerateSeed()
{
    prev::util::RandomNumberGenerator rng{};
    std::uniform_int_distribution<unsigned long> urd(0, 1000000000);
    return urd(rng.GetRandomEngine());
}

float PerlinNoiseGenerator::Interpolate(const float a, const float b, const float blend)
{
    const float theta{ blend * glm::pi<float>() };
    const float factor{ (1.0f - cosf(theta)) * 0.5f };
    return a * (1.0f - factor) + b * factor;
}
} // namespace prev_test::component::terrain