#include "Flare.h"

namespace prev_test::component::sky {
Flare::Flare(const float scale)
    : m_scale{ scale }
{
}

float Flare::GetScale() const
{
    return m_scale;
}
} // namespace prev_test::component::sky