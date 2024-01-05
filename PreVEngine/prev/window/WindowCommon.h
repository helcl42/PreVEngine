#ifndef __WINDOW_COMMON_H__
#define __WINDOW_COMMON_H__

#include <string>

namespace prev::window {
struct WindowCreateInfo {
    std::string title;

    int16_t left;

    int16_t top;

    uint16_t width;

    uint16_t height;

    bool fullScreen;
};
} // namespace prev::window

#endif // !__WINDOW_COMMON_H__
