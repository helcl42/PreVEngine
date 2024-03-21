#ifndef __WINDOW_IMPL_FACTORY_H__
#define __WINDOW_IMPL_FACTORY_H__

#include "WindowImpl.h"

#include <memory>
#include <string>

namespace prev::window::impl {
class WindowImplFactory final {
public:
    std::unique_ptr<WindowImpl> Create(const prev::core::instance::Instance& instance, const WindowInfo& info) const;
};
} // namespace prev::window::impl

#endif // !__WINDOW_IMPL_FACTORY_H__
