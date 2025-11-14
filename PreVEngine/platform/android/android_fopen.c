// From: http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/

// #define _GNU_SOURCE
#define _BSD_SOURCE 1

#include <stdio.h>

#include "android_fopen.h"

#include <android/asset_manager.h>

#include <errno.h>

static int android_read(void* cookie, char* buf, int size)
{
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size)
{
    return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence)
{
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie)
{
    AAsset_close((AAsset*)cookie);
    return 0;
}

// must be established by someone else...
static AAssetManager* g_assetManager = NULL;
void android_fopen_set_asset_manager(AAssetManager* manager)
{
    g_assetManager = manager;
}

FILE* android_fopen(const char* fname, const char* mode)
{
    if (mode[0] == 'w') {
        return NULL;
    }
    AAsset* asset = AAssetManager_open(g_assetManager, fname, 0);
    if (!asset) {
        return NULL;
    }
    return funopen(asset, android_read, android_write, android_seek, android_close);
}

AAsset* android_open_asset(const char* fname, const int mode)
{
    return AAssetManager_open(g_assetManager, fname, mode);
}