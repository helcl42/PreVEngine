#ifndef __ANDROID_NATIVE_H__
#define __ANDROID_NATIVE_H__

#include <android/log.h>
#include <android_native_app_glue.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <streambuf>
#include <string.h>

#include "android_fopen.h" // redirect fopen, to read files from asset folder

extern android_app* g_AndroidApp; // Native Activity state info

int printf(const char* format, ...); // printf for Android (allows multiple printf's per line)
void ShowKeyboard(bool visible, int flags = 0); // Show/hide Android keyboard
int GetUnicodeChar(int eventType, int keyCode, int metaState);

#endif