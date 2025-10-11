#ifndef __WINDOW_COMMON_H__
#define __WINDOW_COMMON_H__

#include <string>

namespace prev::window {
struct WindowCreateInfo {
    bool headless{};

    std::string title{};

    int32_t left{};

    int32_t top{};

    uint32_t width{};

    uint32_t height{};

    bool fullScreen{};
};
} // namespace prev::window

#endif // !__WINDOW_COMMON_H__
