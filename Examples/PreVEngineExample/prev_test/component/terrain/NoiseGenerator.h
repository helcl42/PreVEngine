#ifndef __NOISE_GENERATOR_H__
#define __NOISE_GENERATOR_H__

#include <prev/common/Common.h>

namespace prev_test::component::terrain {
class NoiseGenerator {
public:
    explicit NoiseGenerator();

    explicit NoiseGenerator(const unsigned long seed);

    virtual ~NoiseGenerator() = default;

public:
    float GetInterpolatedNoise(const float x, const float z) const;

private:
    float GetNoise(const int x, const int z) const;

    float GetSmoothNoise(const int x, const int z) const;

private:
    static unsigned long GenerateSeed();

    static float Interpolate(const float a, const float b, const float blend);

private:
    const unsigned long m_seed;
};
} // namespace prev_test::component::terrain

#endif // !__NOISE_GENERATOR_H__
