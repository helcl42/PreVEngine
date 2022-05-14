#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "../General.h"
#include "../component/camera/ICameraComponent.h"
#include "../component/ray_casting/IBoundingVolumeComponent.h"
#include "../component/render/IAnimationRenderComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/input/mouse/MouseEvents.h>
#include <prev/input/touch/TouchEvents.h>
#include <prev/scene/graph/SceneNode.h>

#if defined(__ANDROID__)
#include <android/looper.h>
#include <android/sensor.h>

#include <prev/util/MathUtils.h>
#endif

namespace prev_test::scene {
#if defined(__ANDROID__)
    struct Pose {
        glm::quat orientation;
        glm::vec3 position;
    };

    class AndroidPoseProvider final {
    public:
        AndroidPoseProvider() = default;

        ~AndroidPoseProvider() = default;

    public:
        bool Init()
        {
            if(m_initialized) {
                return false;
            }

            m_running = true;
            m_mainLoopThread = std::thread(&AndroidPoseProvider::MainLoop, this);

            m_initialized = true;
            return true;
        }

        void ShutDown()
        {
            if(!m_initialized) {
                return;
            }

            m_running = false;
            if(m_mainLoopThread.joinable()) {
                m_mainLoopThread.join();
            }

            m_initialized = false;
        }

        Pose GetCurrentPose() const {
            std::scoped_lock lock(m_mutex);
            return { m_currentOrientation, m_currentPosition };
        }

    private:
        void MainLoop()
        {
            ASensorManager* sensorManager = ASensorManager_getInstance();
            if (!sensorManager) {
                LOGE("Failed to get a sensor manager\n");
                return;
            }

            ASensorList sensorList{ nullptr };
            int sensorCount = ASensorManager_getSensorList(sensorManager, &sensorList);
            LOGI("Found %d supported sensors\n", sensorCount);
            for (int i = 0; i < sensorCount; i++) {
                LOGI("HAL supports sensor %s\n", ASensor_getName(sensorList[i]));
            }

            ASensorEventQueue* sensorEventQueue = ASensorManager_createEventQueue(sensorManager, ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS), looperId, nullptr, nullptr);
            if (!sensorEventQueue) {
                LOGE("Failed to create a sensor event queue\n");
                return;
            }

            const std::vector<int> sensorTypes = {
                    ASENSOR_TYPE_GYROSCOPE,
                    ASENSOR_TYPE_LINEAR_ACCELERATION
            };

            std::vector<ASensorRef> sensors;
            for(const auto& sensorType : sensorTypes) {
                ASensorRef sensor;
                bool sensorFound{false};
                for (int i = 0; i < sensorCount; i++) {
                    sensor = sensorList[i];
                    if (ASensor_getType(sensor) != sensorType) {
                        continue;
                    }
                    if (ASensorEventQueue_enableSensor(sensorEventQueue, sensor) < 0) {
                        continue;
                    }
                    if (ASensorEventQueue_setEventRate(sensorEventQueue, sensor, timeoutMicroSeconds) < 0) {
                        LOGE("Failed to set the %s sample rate\n", ASensor_getName(sensor));
                        continue;
                    }
                    sensorFound = true;
                    break;
                }

                if (!sensorFound) {
                    LOGE("No sensor of the specified type found\n");
                    int ret = ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
                    if (ret < 0) {
                        LOGE("Failed to destroy event queue: %s\n", strerror(-ret));
                    }
                    return;
                }

                LOGI("Sensor %s activated\n", ASensor_getName(sensor));
                sensors.push_back(sensor);
            }

            m_lastOrientationTimestamp = 0;
            m_lastAccelerationTimestamp = 0;

            while(m_running) {
                if(ALooper_pollAll(timeoutMicroSeconds,nullptr, nullptr, nullptr) != looperId) {
                    LOGE("Incorrect Looper ident read from poll.\n");
                    continue;
                }

                ASensorEvent event;
                if (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) <= 0) {
                    LOGE("Failed to read data from the sensor.\n");
                    continue;
                }

                if(event.type == ASENSOR_TYPE_GYROSCOPE) {
                    std::scoped_lock lock(m_mutex);
                    if(m_lastOrientationTimestamp != 0) {
                        const auto deltaTime{ static_cast<float>(event.timestamp - m_lastOrientationTimestamp) / 1000000000.0f };
                        const glm::vec3 angularVelocity{ event.data[0], event.data[1], event.data[2] };
                        m_currentOrientation = IntegrateGyroscopeRotation(m_currentOrientation, angularVelocity, deltaTime);
                    }
                    m_lastOrientationTimestamp = event.timestamp;
                } else if(event.type == ASENSOR_TYPE_LINEAR_ACCELERATION) {
                    std::scoped_lock lock(m_mutex);
                    if(m_lastAccelerationTimestamp != 0) {
                        const auto deltaTime{ static_cast<float>(event.timestamp - m_lastAccelerationTimestamp) / 1000000000.0f };

                        const glm::vec3 acceleration{ event.data[0], event.data[1], event.data[2] };

                        // TODO - naive double integral here without any scale & bias calibration
                        const auto velocityDiff{ acceleration * deltaTime };
                        m_currentVelocity += velocityDiff;

                        const auto positionDIff{ m_currentVelocity * deltaTime };
                        m_currentPosition += positionDIff;

                        //LOGE("Got acceleration: (%f, %f, %f), dt: %f", acceleration.x, acceleration.y, acceleration.z, deltaTime);
                    }
                    m_lastAccelerationTimestamp = event.timestamp;
                }
            }

            for(const auto& sensor : sensors) {
                int ret = ASensorEventQueue_disableSensor(sensorEventQueue, sensor);
                if (ret < 0) {
                    LOGE("Failed to disable %s: %s\n", ASensor_getName(sensor), strerror(-ret));
                }
                ret = ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
                if (ret < 0) {
                    LOGE("Failed to destroy event queue: %s\n", strerror(-ret));
                }
            }
        }

    private:
        static glm::quat IntegrateGyroscopeRotation(const glm::quat& previousOrientation, const glm::vec3& angularVelocity, const float dt) {
            const float angularVelocityMagnitude{glm::length(angularVelocity) };
            const glm::vec3 angularVelocityDirection{ glm::normalize(angularVelocity) };

            // construct orientation diff from angular velocity and elapsed time
            const float thetaOverTwo{angularVelocityMagnitude * dt / 2.0f };
            const float sinThetaOverTwo{ std::sin(thetaOverTwo) };
            const float cosThetaOverTwo{ std::cos(thetaOverTwo) };
            const glm::quat orientationDelta{ cosThetaOverTwo, sinThetaOverTwo * angularVelocityDirection.x, sinThetaOverTwo * angularVelocityDirection.y, sinThetaOverTwo * angularVelocityDirection.z };

            // concatenate diff with previous orientation
            return previousOrientation * orientationDelta;
        }

    private:
        glm::quat m_currentOrientation{ 1.0f, 0.0f, 0.0f, 0.0f };

        int64_t m_lastOrientationTimestamp;

        glm::vec3 m_currentPosition;

        glm::vec3 m_currentVelocity;

        int64_t m_lastAccelerationTimestamp;

        std::atomic_bool m_initialized;

        std::atomic_bool m_running;

        std::thread m_mainLoopThread;

        mutable std::mutex m_mutex;

        static const inline int timeoutMicroSeconds{ 10000 };

        static const inline int looperId{ 1 };
    };
#endif

class Player final : public prev::scene::graph::SceneNode {
public:
    Player(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

    virtual ~Player() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

    void operator()(const prev::input::mouse::MouseEvent& mouseEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

    void operator()(const prev::input::mouse::MouseScrollEvent& scrollEvent);

private:
    static const inline float RUN_SPEED{ 14.0f };

    static const inline float YAW_TURN_SPEED{ 3.0f };

    static const inline float PITCH_TURN_SPEED{ 0.5f };

    static const inline float GRAVITY_Y{ -10.0f };

    static const inline float JUMP_POWER{ 4.0f };

    static const inline uint32_t WALKING_ANIMATION_INDEX{ 0 };

    static const inline uint32_t JUMP_ANIMATION_INDEX{ 1 };

private:
    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

    bool m_shouldGoForward{ false };

    bool m_shouldGoBackward{ false };

    bool m_shouldGoLeft{ false };

    bool m_shouldGoRight{ false };

    bool m_shouldRotate{ false };

    float m_upwardSpeed{ 0.0f };

    float m_yawDiff{ 0.0f };

    float m_pitchDiff{ 0.0f };

    bool m_isInTheAir{ false };

    float m_cameraPitch{ -20.0f };

    float m_distanceFromPerson{ 30.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

#if defined(__ANDROID__)
    std::unique_ptr<AndroidPoseProvider> m_poseProvider;
#endif

private:
    prev::event::EventHandler<Player, prev::input::keyboard::KeyEvent> m_keyboardEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::mouse::MouseEvent> m_mouseEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::touch::TouchEvent> m_touchEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::mouse::MouseScrollEvent> m_mouseScrollsHandler{ *this };

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::render::IAnimationRenderComponent> m_animationRenderComponent;

    std::shared_ptr<prev_test::component::camera::ICameraComponent> m_cameraComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__PLAYER_H__
