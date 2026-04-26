#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "../Core.h"

#include <memory>

namespace prev::core::instance {
class Instance {
public:
    Instance(GfxInstance instance);

    ~Instance();

public:
    GfxInstance GetHandle() const;

    operator GfxInstance() const;

private:
    GfxInstance m_instance{};
};
} // namespace prev::core::instance

#endif
