#ifndef __SCENE_CONFIG_H__
#define __SCENE_CONFIG_H__

namespace prev::scene {
struct SceneConfig {
    // swapchain
    bool VSync{ true };

    uint32_t framesInFlight{ 3 };
};
} // namespace prev::scene

#endif // !__SCENE_CONFIG_H__
