#include "TestApp.h"

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

    auto config = std::make_shared<prev::core::EngineConfig>();

    prev_test::TestApp<prev_test::SceneNodeFlags> app(config);
    app.Init();
    app.Run();
    app.ShutDown();

    return 0;
}
