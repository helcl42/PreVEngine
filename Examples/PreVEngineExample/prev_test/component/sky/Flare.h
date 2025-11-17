#ifndef __FLARE_H__
#define __FLARE_H__

namespace prev_test::component::sky {
class Flare {
public:
    Flare(const float scale);

    ~Flare() = default;

public:
    float GetScale() const;

private:
    float m_scale{};
};
} // namespace prev_test::component::sky

#endif // !__FLARE_H__
