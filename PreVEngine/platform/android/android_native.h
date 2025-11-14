#ifndef __ANDROID_NATIVE_H__
#define __ANDROID_NATIVE_H__

#include <android_native_app_glue.h>

#include <stdio.h>

int printf(const char* format, ...); // printf for Android (allows multiple printf's per line)

void android_native_set_app_instance(android_app* app);

android_app* android_native_get_app_instance();

void android_native_show_keyboard(bool visible, int flags = 0); // Show/hide Android keyboard

int android_native_get_unicode_char(int eventType, int keyCode, int metaState);

void android_native_force_finish_activity();

#endif