#ifndef __ANIMATION_FACTORY_H__
#define __ANIMATION_FACTORY_H__

#include "../IAnimation.h"

#include <memory>

namespace prev_test::render::animation {
class AnimationFactory {
public:
    std::unique_ptr<prev_test::render::IAnimation> Create(const std::string& modelPath) const;
};
} // namespace prev_test::render::animation

#endif