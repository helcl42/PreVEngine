#include "AndroidGamepadBackend.h"

#if defined(TARGET_PLATFORM_ANDROID)

#include "../../../../common/Logger.h"

#include <android_native.h>

#include <jni.h>

namespace prev::input::gamepad::impl::android {
namespace {
    constexpr auto SCAN_INTERVAL{ std::chrono::milliseconds(1000) };

    bool ClearException(JNIEnv* env)
    {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            return true;
        }
        return false;
    }
} // namespace

AndroidGamepadBackend::~AndroidGamepadBackend()
{
    if (m_jni.inputDeviceClass) {
        ScopedJniEnv scopedEnv;
        JNIEnv* env{ scopedEnv.Get() };
        if (env) {
            env->DeleteGlobalRef(m_jni.inputDeviceClass);
        }
    }
}

void AndroidGamepadBackend::Poll(GamepadInputs& inputs)
{
    const auto now{ Clock::now() };
    if (m_scanAvailable && now >= m_nextScan) {
        m_nextScan = now + SCAN_INTERVAL;
        std::vector<AndroidInputDevice> devices;
        if (ScanDevices(devices)) {
            m_tracker.SetDeviceList(devices);
        } else {
            LOGW("Gamepad: InputDevice scan unavailable - disconnects will not be detected");
            m_scanAvailable = false;
        }
    }
    m_tracker.Snapshot(inputs);
}

void AndroidGamepadBackend::operator()(const AndroidGamepadKeyEvent& keyEvent)
{
    const bool wasTracked{ m_tracker.IsTracked(keyEvent.deviceId) };
    m_tracker.OnKey(keyEvent.deviceId, keyEvent.keyCode, keyEvent.pressed);
    ScanIfNewlyTracked(keyEvent.deviceId, wasTracked);
}

void AndroidGamepadBackend::operator()(const AndroidGamepadMotionEvent& motionEvent)
{
    const bool wasTracked{ m_tracker.IsTracked(motionEvent.deviceId) };
    m_tracker.OnMotion(motionEvent.deviceId, motionEvent.values);
    ScanIfNewlyTracked(motionEvent.deviceId, wasTracked);
}

void AndroidGamepadBackend::ScanIfNewlyTracked(const int32_t deviceId, const bool wasTracked)
{
    if (!wasTracked && m_tracker.IsTracked(deviceId)) {
        m_nextScan = Clock::time_point{};
    }
}

bool AndroidGamepadBackend::ScanDevices(std::vector<AndroidInputDevice>& outDevices)
{
    ScopedJniEnv scopedEnv;
    JNIEnv* env{ scopedEnv.Get() };
    if (!env || !LoadInputDeviceJni(env) || env->PushLocalFrame(64) != JNI_OK) {
        return false;
    }

    bool ok{ false };
    const auto ids{ static_cast<jintArray>(env->CallStaticObjectMethod(m_jni.inputDeviceClass, m_jni.getDeviceIds)) };
    if (ids && !ClearException(env)) {
        const jsize count{ env->GetArrayLength(ids) };
        std::vector<jint> deviceIds(static_cast<size_t>(count));
        env->GetIntArrayRegion(ids, 0, count, deviceIds.data());

        for (const jint deviceId : deviceIds) {
            const jobject device{ env->CallStaticObjectMethod(m_jni.inputDeviceClass, m_jni.getDevice, deviceId) };
            if (ClearException(env) || !device) {
                continue;
            }
            if (const auto entry{ ReadDevice(env, device, deviceId) }) {
                outDevices.push_back(*entry);
            }
            env->DeleteLocalRef(device);
        }
        ok = true;
    }
    ClearException(env);
    env->PopLocalFrame(nullptr);
    return ok;
}

bool AndroidGamepadBackend::LoadInputDeviceJni(JNIEnv* env)
{
    if (m_jni.inputDeviceClass) {
        return true;
    }

    const jclass inputDeviceClass{ env->FindClass("android/view/InputDevice") };
    if (ClearException(env) || !inputDeviceClass) {
        return false;
    }

    const auto staticMethod = [&](const char* name, const char* signature) {
        const jmethodID id{ env->GetStaticMethodID(inputDeviceClass, name, signature) };
        return ClearException(env) ? nullptr : id;
    };
    const auto method = [&](const char* name, const char* signature) {
        const jmethodID id{ env->GetMethodID(inputDeviceClass, name, signature) };
        return ClearException(env) ? nullptr : id;
    };

    InputDeviceJni jni{};
    jni.getDeviceIds = staticMethod("getDeviceIds", "()[I");
    jni.getDevice = staticMethod("getDevice", "(I)Landroid/view/InputDevice;");
    jni.getSources = method("getSources", "()I");
    jni.getName = method("getName", "()Ljava/lang/String;");
    jni.isVirtual = method("isVirtual", "()Z");
    jni.getMotionRange = method("getMotionRange", "(I)Landroid/view/InputDevice$MotionRange;");
    if (jni.getDeviceIds && jni.getDevice && jni.getSources && jni.getName && jni.isVirtual && jni.getMotionRange) {
        jni.inputDeviceClass = static_cast<jclass>(env->NewGlobalRef(inputDeviceClass));
    }
    env->DeleteLocalRef(inputDeviceClass);

    if (!jni.inputDeviceClass) {
        return false;
    }
    m_jni = jni;
    return true;
}

std::optional<AndroidInputDevice> AndroidGamepadBackend::ReadDevice(JNIEnv* env, const jobject device, const jint deviceId) const
{
    const jint sources{ env->CallIntMethod(device, m_jni.getSources) };
    if (ClearException(env) || !IsGamepadSource(sources)) {
        return std::nullopt;
    }
    const jboolean isVirtual{ env->CallBooleanMethod(device, m_jni.isVirtual) };
    if (ClearException(env) || isVirtual) {
        return std::nullopt;
    }

    // A stick tells a pad apart from a remote or keyboard that also reports the gamepad source.
    const jobject xRange{ env->CallObjectMethod(device, m_jni.getMotionRange, AMOTION_EVENT_AXIS_X) };
    if (ClearException(env) || !xRange) {
        return std::nullopt;
    }
    env->DeleteLocalRef(xRange);

    AndroidInputDevice entry{ deviceId, {} };
    const auto name{ static_cast<jstring>(env->CallObjectMethod(device, m_jni.getName)) };
    if (!ClearException(env) && name) {
        if (const char* chars{ env->GetStringUTFChars(name, nullptr) }) {
            entry.name = chars;
            env->ReleaseStringUTFChars(name, chars);
        }
        env->DeleteLocalRef(name);
    }

    const jobject zRange{ env->CallObjectMethod(device, m_jni.getMotionRange, AMOTION_EVENT_AXIS_Z) };
    const bool hasZ{ !ClearException(env) && zRange };
    const jobject rxRange{ env->CallObjectMethod(device, m_jni.getMotionRange, AMOTION_EVENT_AXIS_RX) };
    const bool hasRx{ !ClearException(env) && rxRange };
    entry.rightStickOnRxRy = !hasZ && hasRx;
    if (zRange) {
        env->DeleteLocalRef(zRange);
    }
    if (rxRange) {
        env->DeleteLocalRef(rxRange);
    }
    return entry;
}
} // namespace prev::input::gamepad::impl::android

#endif
