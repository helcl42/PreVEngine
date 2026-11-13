#ifndef __ANDROID_NATIVE_H__
#define __ANDROID_NATIVE_H__

#include <android_native_app_glue.h>

#include <android/asset_manager.h>

#include <jni.h>

#include <stdio.h>

/// hijack fopen and route it through the android asset system so that we can pull things out of our package's APK
/// From http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/
void android_fopen_set_asset_manager(AAssetManager* manager);

FILE* android_fopen(const char* fname, const char* mode);

#define fopen(name, mode) android_fopen(name, mode)

AAsset* android_open_asset(const char* fname, const int mode);

int printf(const char* format, ...); // printf for Android (allows multiple printf's per line)

void android_native_set_app_instance(android_app* app);

android_app* android_native_get_app_instance();

void android_native_show_keyboard(bool visible, int flags = 0); // Show/hide Android keyboard

int android_native_get_unicode_char(int eventType, int keyCode, int metaState);

// Attaches the calling thread to the JVM for the scope; detaches only if it was not attached before.
class ScopedJniEnv final {
public:
    ScopedJniEnv();

    ~ScopedJniEnv();

    ScopedJniEnv(const ScopedJniEnv&) = delete;

    ScopedJniEnv& operator=(const ScopedJniEnv&) = delete;

    JNIEnv* Get() const;

private:
    JavaVM* m_vm{ nullptr };

    JNIEnv* m_env{ nullptr };

    bool m_attached{ false };
};

#endif