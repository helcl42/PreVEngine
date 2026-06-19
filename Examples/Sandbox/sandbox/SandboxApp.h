#ifndef __SANDBOX_APP_H__
#define __SANDBOX_APP_H__

#include <prev/App.h>

namespace sandbox {
class SandboxApp final : public prev::App {
public:
    SandboxApp(const prev::core::engine::Config& config);

    ~SandboxApp() = default;

protected:
    std::unique_ptr<prev::scene::IScene> CreateScene() const override;

    std::unique_ptr<prev::render::IRootRenderer> CreateRootRenderer() const override;
};
} // namespace sandbox

#endif // !__SANDBOX_APP_H__
