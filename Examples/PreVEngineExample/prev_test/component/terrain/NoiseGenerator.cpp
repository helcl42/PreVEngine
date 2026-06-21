#include "NoiseGenerator.h"

#include <prev/util/Utils.h>

#include <cmath>
#include <cstdint>

namespace prev_test::component::terrain {
NoiseGenerator::NoiseGenerator()
    : m_seed{ GenerateSeed() }
{
}

NoiseGenerator::NoiseGenerator(const unsigned long seed)
    : m_seed{ seed }
{
}

float NoiseGenerator::GetInterpolatedNoise(const float x, const float z) const
{
    // floor (not truncation) so negative coordinates pick the correct lattice cell and a fractional
    // blend in [0, 1); static_cast<int>() truncates toward zero and would mis-interpolate for x/z < 0.
    const int intX{ static_cast<int>(std::floor(x)) };
    const int intZ{ static_cast<int>(std::floor(z)) };
    const float fracX{ x - static_cast<float>(intX) };
    const float fracZ{ z - static_cast<float>(intZ) };

    const float v1{ GetSmoothNoise(intX, intZ) };
    const float v2{ GetSmoothNoise(intX + 1, intZ) };
    const float v3{ GetSmoothNoise(intX, intZ + 1) };
    const float v4{ GetSmoothNoise(intX + 1, intZ + 1) };

    const float i1{ Interpolate(v1, v2, fracX) };
    const float i2{ Interpolate(v3, v4, fracX) };
    return Interpolate(i1, i2, fracZ);
}

float NoiseGenerator::GetNoise(const int x, const int z) const
{
    // Deterministic, platform-independent value noise: pure fixed-width unsigned integer math (with
    // well-defined modular wraparound) finalized to a float in [-1, 1). This replaces a per-call
    // std::default_random_engine + std::uniform_real_distribution, both of which are
    // implementation-defined and so produced a different terrain shape on each platform/stdlib.
    uint32_t h{ static_cast<uint32_t>(x) * 0x8da6b343u
        + static_cast<uint32_t>(z) * 0xd8163841u
        + static_cast<uint32_t>(m_seed) * 0xcb1ab31fu };
    // lowbias32 avalanche finalizer
    h ^= h >> 16;
    h *= 0x7feb352du;
    h ^= h >> 15;
    h *= 0x846ca68bu;
    h ^= h >> 16;
    const float unit{ static_cast<float>(h >> 8) / static_cast<float>(0x01000000u) }; // top 24 bits -> [0, 1)
    return unit * 2.0f - 1.0f;
}

float NoiseGenerator::GetSmoothNoise(const int x, const int z) const
{
    const float corners{ (GetNoise(x - 1, z - 1) + GetNoise(x + 1, z - 1) + GetNoise(x - 1, z + 1) + GetNoise(x + 1, z + 1)) / 16.0f };
    const float sides{ (GetNoise(x - 1, z) + GetNoise(x + 1, z) + GetNoise(x, z - 1) + GetNoise(x, z + 1)) / 8.0f };
    const float center{ GetNoise(x, z) / 4.0f };
    return corners + sides + center;
}

unsigned long NoiseGenerator::GenerateSeed()
{
    prev::util::RandomNumberGenerator rng{};
    std::uniform_int_distribution<unsigned long> urd(0, 1000000000);
    return urd(rng.GetRandomEngine());
}

float NoiseGenerator::Interpolate(const float a, const float b, const float blend)
{
    const float theta{ blend * glm::pi<float>() };
    const float factor{ (1.0f - cosf(theta)) * 0.5f };
    return a * (1.0f - factor) + b * factor;
}
} // namespace prev_test::component::terrain