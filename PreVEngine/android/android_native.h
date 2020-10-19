#ifndef __ANDROID_NATIVE_H__
#define __ANDROID_NATIVE_H__

#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <streambuf>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <vulkan_wrapper.h>                      // Builds dispatch table for Vulkan functions
#include "android_fopen.h"                       // redirect fopen, to read files from asset folder

extern android_app* g_AndroidApp;                 // Native Activity state info

class stdiobuf : public std::streambuf {
public:
    stdiobuf(FILE* file)
            : m_file(file) {}

    ~stdiobuf() {
        if (this->m_file) {
            fclose(this->m_file);
        }
    }

    int underflow() {
        if (this->gptr() == this->egptr() && this->m_file) {
            size_t size = fread(this->m_buffer, 1, BUFFER_SIZE,  this->m_file);
            this->setg(this->m_buffer, this->m_buffer, this->m_buffer + size);
        }
        return this->gptr() == this->egptr() ? traits_type::eof() : traits_type::to_int_type(*this->gptr());
    }

private:
    static const inline size_t BUFFER_SIZE{ 8192 };

    FILE* m_file;

    char  m_buffer[BUFFER_SIZE];
};

int  printf(const char* format, ...);            // printf for Android (allows multiple printf's per line)
void ShowKeyboard(bool visible, int flags = 0);  // Show/hide Android keyboard
int  GetUnicodeChar(int eventType, int keyCode, int metaState);

#endif