#include <../platform/android/android_fopen.h>
#include <../platform/android/android_native.h>

// Forward declaration of main function
int PreVMain(int argc, char** argv);

void android_main(struct android_app* state)
{
    android_native_set_app_instance(state); // Set global android app instance
    android_fopen_set_asset_manager(state->activity->assetManager); // Re-direct fopen to read assets from our APK.

    PreVMain(0, NULL); // call the common main

    ANativeActivity_finish(state->activity);

    // android_native_force_finish_activity();
    // exit(0);
}