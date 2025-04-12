#ifndef __ICOMPONENT_H__
#define __ICOMPONENT_H__

#include <string>

namespace prev::scene::component {
class IComponent {
public:
    virtual ~IComponent() = default;
};
} // namespace prev::scene::component

#endif