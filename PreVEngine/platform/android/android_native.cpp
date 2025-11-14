#include "android_native.h"

#include <android/log.h>

#include <jni.h>

#include <stdio.h>
#include <stdlib.h>
#include <streambuf>
#include <string.h>

//----------------------------------------printf for Android---------------------
// Uses a 256 byte buffer to allow concatenating multiple printf's onto one log line.
// The buffer gets flushed when the printf string ends in a '\n', or the buffer is full.
// Alternative with no concatenation:
//   #define printf(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)

struct printBuf {
    static const int SIZE = 256;
    char buf[SIZE];
    printBuf() { clear(); }
    printBuf(const char* c)
    {
        memset(buf, 0, SIZE);
        strncpy(buf, c, SIZE - 1);
    }
    printBuf& operator+=(const char* c)
    {
        strncat(buf, c, SIZE - len() - 1);
        if (len() >= SIZE - 1)
            flush();
        return *this;
    }
    int len() { return strlen(buf); }
    void clear() { memset(buf, 0, SIZE); }
    void flush()
    {
        __android_log_print(ANDROID_LOG_INFO, "PreVEngine", "%s", buf);
        clear();
    }
} printBuf;

int printf(const char* format, ...)
{ // printf for Android
    char buf[printBuf.SIZE];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buf, sizeof(buf), format, argptr);
    va_end(argptr);
    printBuf += buf;
    int len = strlen(buf);
    if ((len >= printBuf.SIZE - 1) || (buf[len - 1] == '\n'))
        printBuf.flush(); // flush on
    return strlen(buf);
}
//--------------------------------------------------------------------------------------------------

static android_app* g_androidApp = NULL;
void android_native_set_app_instance(android_app* app)
{
    g_androidApp = app;
}

android_app* android_native_get_app_instance()
{
    return g_androidApp;
}

#define CALL_OBJ_METHOD(OBJ, METHOD, SIGNATURE, ...) jniEnv->CallObjectMethod(OBJ, jniEnv->GetMethodID(jniEnv->GetObjectClass(OBJ), METHOD, SIGNATURE), __VA_ARGS__)
#define CALL_BOOL_METHOD(OBJ, METHOD, SIGNATURE, ...) jniEnv->CallBooleanMethod(OBJ, jniEnv->GetMethodID(jniEnv->GetObjectClass(OBJ), METHOD, SIGNATURE), __VA_ARGS__)

void android_native_show_keyboard(bool visible, int flags)
{
    // Attach current thread to the JVM.
    JavaVM* javaVM = g_androidApp->activity->vm;
    JNIEnv* jniEnv = g_androidApp->activity->env;
    JavaVMAttachArgs Args = { JNI_VERSION_1_6, "NativeThread", NULL };
    jint result = javaVM->AttachCurrentThread(&jniEnv, &Args);
    if (result == JNI_ERR) {
        return;
    }

    // Retrieve NativeActivity.
    jobject lNativeActivity = g_androidApp->activity->clazz;

    // Retrieve Context.INPUT_METHOD_SERVICE.
    jclass ClassContext = jniEnv->FindClass("android/content/Context");
    jfieldID FieldINPUT_METHOD_SERVICE = jniEnv->GetStaticFieldID(ClassContext, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
    jobject INPUT_METHOD_SERVICE = jniEnv->GetStaticObjectField(ClassContext, FieldINPUT_METHOD_SERVICE);

    // getSystemService(Context.INPUT_METHOD_SERVICE).
    jobject lInputMethodManager = CALL_OBJ_METHOD(lNativeActivity, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;", INPUT_METHOD_SERVICE);

    // getWindow().getDecorView().
    jobject lWindow = CALL_OBJ_METHOD(lNativeActivity, "getWindow", "()Landroid/view/Window;", 0);
    jobject lDecorView = CALL_OBJ_METHOD(lWindow, "getDecorView", "()Landroid/view/View;", 0);
    if (visible) {
        jboolean lResult = CALL_BOOL_METHOD(lInputMethodManager, "showSoftInput", "(Landroid/view/View;I)Z", lDecorView, flags);
    } else {
        jobject lBinder = CALL_OBJ_METHOD(lDecorView, "getWindowToken", "()Landroid/os/IBinder;", 0);
        jboolean lResult = CALL_BOOL_METHOD(lInputMethodManager, "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z", lBinder, flags);
    }
    // Finished with the JVM.
    javaVM->DetachCurrentThread();
}

int android_native_get_unicode_char(int eventType, int keyCode, int metaState)
{
    JavaVM* javaVM = g_androidApp->activity->vm;
    JNIEnv* jniEnv = g_androidApp->activity->env;

    JavaVMAttachArgs Args = { JNI_VERSION_1_6, "NativeThread", NULL };
    jint result = javaVM->AttachCurrentThread(&jniEnv, &Args);
    if (result == JNI_ERR) {
        return 0;
    }

    jclass class_key_event = jniEnv->FindClass("android/view/KeyEvent");

    jmethodID method_get_unicode_char = jniEnv->GetMethodID(class_key_event, "getUnicodeChar", "(I)I");
    jmethodID eventConstructor = jniEnv->GetMethodID(class_key_event, "<init>", "(II)V");
    jobject eventObj = jniEnv->NewObject(class_key_event, eventConstructor, eventType, keyCode);
    int unicodeKey = jniEnv->CallIntMethod(eventObj, method_get_unicode_char, metaState);

    javaVM->DetachCurrentThread();

    // LOGI("Keycode: %d  MetaState: %d Unicode: %d", keyCode, metaState, unicodeKey);
    return unicodeKey;
}

void android_native_force_finish_activity()
{
    JavaVM* javaVM = g_androidApp->activity->vm;
    JNIEnv* jniEnv = g_androidApp->activity->env;
    JavaVMAttachArgs Args = { JNI_VERSION_1_6, "NativeThread", NULL };
    javaVM->AttachCurrentThread(&jniEnv, &Args);

    jclass classActivity = jniEnv->GetObjectClass(g_androidApp->activity->clazz);
    jmethodID activityFinishID = jniEnv->GetMethodID(classActivity, "finish", "()V");

    signal(SIGABRT, SIG_DFL);

    jniEnv->CallVoidMethod(g_androidApp->activity->clazz, activityFinishID);

    javaVM->DetachCurrentThread();

    pthread_exit(NULL);
}
