#include "Root.h"

#include "Cube.h"
#include "Fire.h"
#include "Plane.h"
#include "Player.h"
#include "Stone.h"
#include "Text.h"
#include "Time.h"
#include "light/Light.h"
#include "light/MainLight.h"
#include "ray_casting/InputsHelper.h"
#include "ray_casting/RayCaster.h"
#include "ray_casting/RayCasterObserver.h"
#include "robot/CubeRobot.h"
#include "shadow/Shadow.h"
#include "sky/LensFlare.h"
#include "sky/Sky.h"
#include "sky/SkyBox.h"
#include "sky/Sun.h"
#include "terrain/TerrainManager.h"
#include "water/WaterManager.h"

#include "../common/AssetManager.h"

#include <prev/util/Utils.h>

namespace prev_test::scene {
Root::Root()
    : SceneNode()
{
}

void Root::Init()
{
    auto timeNode = std::make_shared<Time>();
    AddChild(timeNode);

    auto inputsHelper = std::make_shared<ray_casting::InputsHelper>();
    AddChild(inputsHelper);

    // auto skyBox = std::make_shared<sky::SkyBox>();
    // AddChild(skyBox);

    auto sky = std::make_shared<sky::Sky>();
    AddChild(sky);

    auto sunLight = std::make_shared<light::MainLight>(glm::vec3(15000.0f, 5000.0f, 15000.0f));
    sunLight->SetTags({ TAG_MAIN_LIGHT, TAG_LIGHT });
    AddChild(sunLight);

    // auto light1 = std::make_shared<light::Light>(glm::vec3(30.0f, 20.0f, 35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // light1->SetTags({ TAG_LIGHT });
    // AddChild(light1);
    //
    // auto light2 = std::make_shared<light::Light>(glm::vec3(-30.0f, 20.0f, 35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // light2->SetTags({ TAG_LIGHT });
    // AddChild(light2);
    //
    // auto light3 = std::make_shared<light::Light>(glm::vec3(0.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // light3->SetTags({ TAG_LIGHT });
    // AddChild(light3);

    auto shadows = std::make_shared<shadow::Shadows>();
    shadows->SetTags({ TAG_SHADOW });
    AddChild(shadows);

    // auto freeCamera = std::make_shared<Camera>();
    // freeCamera->SetTags({ TAG_MAIN_CAMERA });
    // AddChild(freeCamera);

    const int32_t MAX_GENERATED_HEIGHT{ 1 };
    const float DISTANCE{ 40.0f };
    for (int32_t i = 0; i <= MAX_GENERATED_HEIGHT; i++) {
        for (int32_t j = 0; j <= MAX_GENERATED_HEIGHT; j++) {
            for (int32_t k = 0; k <= MAX_GENERATED_HEIGHT; k++) {
                auto robot = std::make_shared<robot::CubeRobot>(glm::vec3(i * DISTANCE, j * DISTANCE, k * DISTANCE), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
                AddChild(robot);
            }
        }
    }

    auto player = std::make_shared<Player>(glm::vec3(0.0f), glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))), glm::vec3(0.06f));
    player->SetTags({ TAG_MAIN_CAMERA, TAG_PLAYER });
    AddChild(player);

    const uint32_t TERRAIN_GRID_MAX_X{ 3 };
    const uint32_t TERRAIN_GRID_MAX_Z{ 3 };
    auto terrainManager = std::make_shared<terrain::TerrainManager>(TERRAIN_GRID_MAX_X, TERRAIN_GRID_MAX_Z);
    AddChild(terrainManager);

    auto water = std::make_shared<water::WaterManager>(TERRAIN_GRID_MAX_X, TERRAIN_GRID_MAX_Z);
    AddChild(water);

    auto sun = std::make_shared<sky::Sun>();
    AddChild(sun);

    const float ITEMS_TERRAIN_BORDER_PADDING{ 10.0f };

    prev::util::RandomNumberGenerator rng{};
    std::uniform_real_distribution<float> positionDistribution(ITEMS_TERRAIN_BORDER_PADDING, prev_test::component::terrain::TERRAIN_TILE_SIZE * TERRAIN_GRID_MAX_X - ITEMS_TERRAIN_BORDER_PADDING);
    std::uniform_real_distribution<float> scaleDistribution(0.005f, 0.01f);

    const uint32_t STONES_COUNT{ 12 };
    for (uint32_t i = 0; i < STONES_COUNT; i++) {
        const auto x{ positionDistribution(rng.GetRandomEngine()) };
        const auto z{ positionDistribution(rng.GetRandomEngine()) };
        auto stone = std::make_shared<Stone>(glm::vec3(x, 0.0f, z), glm::quat(glm::vec3(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f)))), glm::vec3(scaleDistribution(rng.GetRandomEngine())));
        AddChild(stone);
    }

    auto fire = std::make_shared<Fire>(glm::vec3(30.0f, 0.0f, 100.0f));
    AddChild(fire);

    // auto cube1 = std::make_shared<Cube>(glm::vec3(-35.0f, 0.0f, -35.0f), glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f))), glm::vec3(20.0f), AssetManager::Instance().GetAssetPath("Textures/example_1_texture.png"), AssetManager::Instance().GetAssetPath("Textures/example_1_normal.png"), AssetManager::Instance().GetAssetPath("Textures/ouput_cv.png"), 0.1f);
    // AddChild(cube1);

    // auto cube2 = std::make_shared<Cube>(glm::vec3(-65.0f, 0.0f, -65.0f), glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 90.0f))), glm::vec3(20.0f), AssetManager::Instance().GetAssetPath("Textures/rock.png"), AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), AssetManager::Instance().GetAssetPath("Textures/rock_cone.png"), 0.1f);
    // AddChild(cube2);

    // auto cube3 = std::make_shared<Cube>(glm::vec3(-10.0f, 0.0f, -110.0f), glm::quat(glm::radians(glm::vec3(90.0f, 90.0f, 0.0f))), glm::vec3(20.0f), AssetManager::Instance().GetAssetPath("Textures/fungus.png"), AssetManager::Instance().GetAssetPath("Textures/fungus_normal_2.png"), AssetManager::Instance().GetAssetPath("Textures/fungus_cone.png"), 0.05f);
    // AddChild(cube3);

    // auto cube4 = std::make_shared<Cube>(glm::vec3(-120.0f, 0.0f, -50.0f), glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f))), glm::vec3(20.0f), AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal_2.png"), AssetManager::Instance().GetAssetPath("Textures/sand_grass_cone.png"), 0.05f);
    // AddChild(cube4);

    // auto cube5 = std::make_shared<Cube>(glm::vec3(-90.0f, 0.0f, -90.0f), glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))), glm::vec3(20.0f), AssetManager::Instance().GetAssetPath("Textures/sand.png"), AssetManager::Instance().GetAssetPath("Textures/sand_normal_2.png"), AssetManager::Instance().GetAssetPath("Textures/sand_cone.png"), 0.1f);
    // AddChild(cube5);

    // auto compute = std::make_shared<ComputeNode>();
    // AddChild(compute);

    auto lensFlare = std::make_shared<sky::LensFlare>();
    AddChild(lensFlare);

    auto text = std::make_shared<Text>();
    AddChild(text);

    auto rayCaster = std::make_shared<ray_casting::RayCaster>();
    AddChild(rayCaster);

    auto rayCastObserver = std::make_shared<ray_casting::RayCastObserver>();
    AddChild(rayCastObserver);

    SceneNode::Init();
}

void Root::Update(float deltaTime)
{
    if (m_addNode) {
        AddNode();
    }

    if (m_removeNode) {
        RemoveNode();
    }

    if (m_reloadAll) {
        LOGW("Reloading whole scene.");
        ShutDown();
        Init();
    }

    m_addNode = m_removeNode = m_reloadAll = false;

    SceneNode::Update(deltaTime);
}

void Root::ShutDown()
{
    SceneNode::ShutDown();
}

void Root::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_J) {
            m_removeNode = true;
        } else if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_K) {
            m_addNode = true;
        } else if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_I) {
            m_reloadAll = true;
        }
    }
}

void Root::operator()(const prev::input::touch::TouchEvent& touchEvent)
{
    if (touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        // m_addNode = true;
    } else if (touchEvent.action == prev::input::touch::TouchActionType::UP) {
        // m_removeNode = true;
    }
}

void Root::AddNode()
{
    const float DISTANCE{ 40.0f };

    prev::util::RandomNumberGenerator rng{};
    std::uniform_int_distribution<> dist(0, 4);

    const auto i{ dist(rng.GetRandomEngine()) };
    const auto j{ dist(rng.GetRandomEngine()) };
    const auto k{ dist(rng.GetRandomEngine()) };

    auto robot = std::make_shared<robot::CubeRobot>(glm::vec3(i * DISTANCE, j * DISTANCE, k * DISTANCE), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
    robot->Init();
    AddChild(robot);
}

void Root::RemoveNode()
{
    if (m_children.size() > 0) {
        auto& child = m_children.back();
        child->ShutDown();
        RemoveChild(child);
    }
}
} // namespace prev_test::scene