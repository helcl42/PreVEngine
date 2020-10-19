// From http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/

#ifndef __ANDROID_FOPEN_H__
#define __ANDROID_FOPEN_H__

#include <stdio.h>
#include <android/asset_manager.h>

#ifdef __cplusplus
extern "C" {
#endif

/* hijack fopen and route it through the android asset system so that
   we can pull things out of our package's APK */

void android_fopen_set_asset_manager(AAssetManager* manager);
FILE* android_fopen(const char* fname, const char* mode);

#define fopen(name, mode) android_fopen(name, mode)

AAsset* android_open_asset(const char* fname, const int mode);

#ifdef __cplusplus
}
#endif

#endif

