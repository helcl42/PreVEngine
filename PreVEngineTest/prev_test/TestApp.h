#ifndef __TEST_APP_H__
#define __TEST_APP_H__

#include <prev/App.h>

namespace prev_test {
class TestApp final : public prev::App {
public:
    TestApp(const prev::core::EngineConfig& config);

    ~TestApp() = default;

protected:
    std::shared_ptr<prev::scene::IScene> CreateScene() const override;

    std::shared_ptr<prev::render::IRootRenderer> CreateRootRenderer() const override;
};
} // namespace prev_test

#endif // !__PREV_TEST_APP_H__
