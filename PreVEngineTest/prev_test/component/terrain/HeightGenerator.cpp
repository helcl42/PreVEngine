#include "HeightGenerator.h"

namespace prev_test::component::terrain {
HeightGenerator::HeightGenerator()
    : m_xOffset(0)
    , m_zOffset(0)
    , m_noiseGenerator(std::make_unique<PerlinNoiseGenerator>())
{
}

HeightGenerator::HeightGenerator(const int x, const int z, const int size, const unsigned int seed)
    : m_xOffset(x * (size - 1))
    , m_zOffset(z * (size - 1))
    , m_noiseGenerator(std::make_unique<PerlinNoiseGenerator>(seed))
{
}

float HeightGenerator::GenerateHeight(const int x, const int z) const
{
    const float d = static_cast<float>(powf(2.0f, static_cast<float>(HeightGenerator::OCTAVES - 1)));

    float total{ 0.0f };
    for (int i = 0; i < HeightGenerator::OCTAVES; i++) {
        const float freq = static_cast<float>(powf(2, static_cast<float>(i)) / d);
        const float amp = static_cast<float>(powf(HeightGenerator::ROUGHNESS, static_cast<float>(i))) * static_cast<float>(HeightGenerator::AMPLITUDE);
        total += m_noiseGenerator->GetInterpolatedNoise((x + m_xOffset) * freq, (z + m_zOffset) * freq) * amp;
    }
    return total;
}
} // namespace prev_test::component::terrain