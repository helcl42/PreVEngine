#include "ShadowsCascade.h"

namespace prev_test::component::shadow {
glm::mat4 ShadowsCascadeFrameData::GetBiasedViewProjectionMatrix(bool flipY) const
{
    static const glm::mat4 biasMat(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    static const glm::mat4 biasMatFlipped(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    if (flipY) {
        glm::mat4 adjustedProj = projectionMatrix;
        adjustedProj[1][1] *= -1.0f;
        return biasMatFlipped * adjustedProj * viewMatrix;
    }
    return biasMat * projectionMatrix * viewMatrix;
}
} // namespace prev_test::component::shadow