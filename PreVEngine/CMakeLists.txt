cmake_minimum_required(VERSION 3.10)

project(PreVEngine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

file(GLOB ROOT_SRC_LIST "prev/*.h" "prev/*.cpp")
file(GLOB COMMON_SRC_LIST "prev/common/*.h" "prev/common/*.cpp")
file(GLOB COMMON_PATTERN_SRC_LIST "prev/common/pattern/*.h" "prev/common/pattern/*.cpp")
file(GLOB CORE_SRC_LIST "prev/core/*.h" "prev/core/*.cpp")
file(GLOB CORE_MEMORY_SRC_LIST "prev/core/memory/*.h" "prev/core/memory/*.cpp")
file(GLOB CORE_DEVICE_SRC_LIST "prev/core/device/*.h" "prev/core/device/*.cpp")
file(GLOB CORE_INSTANCE_SRC_LIST "prev/core/instance/*.h" "prev/core/instance/*.cpp")
file(GLOB EVENT_SRC_LIST "prev/event/*.h" "prev/event/*.cpp")
file(GLOB EVENT_INTERNAL_SRC_LIST "prev/event/internal/*.h" "prev/event/internal/*.cpp")
file(GLOB INPUT_SRC_LIST "prev/input/*.h" "prev/input/*.cpp")
file(GLOB INPUT_MOUSE_SRC_LIST "prev/input/mouse/*.h" "prev/input/mouse/*.cpp")
file(GLOB INPUT_KEYBOARD_SRC_LIST "prev/input/keyboard/*.h" "prev/input/keyboard/*.cpp")
file(GLOB INPUT_TOUCH_SRC_LIST "prev/input/touch/*.h" "prev/input/touch/*.cpp")
file(GLOB RENDER_SRC_LIST "prev/render/*.h" "prev/render/*.cpp")
file(GLOB RENDER_BUFFER_SRC_LIST "prev/render/buffer/*.h" "prev/render/buffer/*.cpp")
file(GLOB RENDER_BUFFER_IMAGE_SRC_LIST "prev/render/buffer/image/*.h" "prev/render/buffer/image/*.cpp")
file(GLOB RENDER_IMAGE_SRC_LIST "prev/render/image/*.h" "prev/render/image/*.cpp")
file(GLOB RENDER_PASS_SRC_LIST "prev/render/pass/*.h" "prev/render/pass/*.cpp")
file(GLOB RENDER_SHADER_SRC_LIST "prev/render/shader/*.h" "prev/render/shader/*.cpp")
file(GLOB RENDER_SAMPLER_SRC_LIST "prev/render/sampler/*.h" "prev/render/sampler/*.cpp")
file(GLOB SCENE_SRC_LIST "prev/scene/*.h" "prev/scene/*.cpp")
file(GLOB SCENE_COMPONENT_SRC_LIST "prev/scene/component/*.h" "prev/scene/component/*.cpp")
file(GLOB SCENE_GRAPH_SRC_LIST "prev/scene/graph/*.h" "prev/scene/graph/*.cpp")
file(GLOB UTIL_SRC_LIST "prev/util/*.h" "prev/util/*.cpp")
file(GLOB WINDOW_SRC_LIST "prev/window/*.h" "prev/window/*.cpp")
file(GLOB WINDOW_IMPL_SRC_LIST "prev/window/impl/*.h" "prev/window/impl/*.cpp")
file(GLOB WINDOW_IMPL_WINDOWS_SRC_LIST "prev/window/impl/windows/*.h" "prev/window/impl/windows/*.cpp")
file(GLOB WINDOW_IMPL_LINUX_SRC_LIST "prev/window/impl/xcb/*.h" "prev/window/impl/xcb/*.cpp")
file(GLOB WINDOW_IMPL_ANDROID_SRC_LIST "prev/window/impl/android/*.h" "prev/window/impl/android/*.cpp")

source_group("prev" FILES ${ROOT_SRC_LIST})
source_group("prev/common" FILES ${COMMON_SRC_LIST})
source_group("prev/common/pattern" FILES ${COMMON_PATTERN_SRC_LIST})
source_group("prev/core" FILES ${CORE_SRC_LIST})
source_group("prev/core/memory" FILES ${CORE_MEMORY_SRC_LIST})
source_group("prev/render/buffer" FILES ${RENDER_BUFFER_SRC_LIST})
source_group("prev/render/buffer/image" FILES ${RENDER_BUFFER_IMAGE_SRC_LIST})
source_group("prev/core/device" FILES ${CORE_DEVICE_SRC_LIST})
source_group("prev/core/instance" FILES ${CORE_INSTANCE_SRC_LIST})
source_group("prev/event" FILES ${EVENT_SRC_LIST})
source_group("prev/event/internal" FILES ${EVENT_INTERNAL_SRC_LIST})
source_group("prev/input" FILES ${INPUT_SRC_LIST})
source_group("prev/input/mouse" FILES ${INPUT_MOUSE_SRC_LIST})
source_group("prev/input/keyboard" FILES ${INPUT_KEYBOARD_SRC_LIST})
source_group("prev/input/touch" FILES ${INPUT_TOUCH_SRC_LIST})
source_group("prev/render" FILES ${RENDER_SRC_LIST})
source_group("prev/render/image" FILES ${RENDER_IMAGE_SRC_LIST})
source_group("prev/render/pass" FILES ${RENDER_PASS_SRC_LIST})
source_group("prev/render/shader" FILES ${RENDER_SHADER_SRC_LIST})
source_group("prev/render/sampler" FILES ${RENDER_SAMPLER_SRC_LIST})
source_group("prev/scene" FILES ${SCENE_SRC_LIST})
source_group("prev/scene/component" FILES ${SCENE_COMPONENT_SRC_LIST})
source_group("prev/scene/graph" FILES ${SCENE_GRAPH_SRC_LIST})
source_group("prev/util" FILES ${UTIL_SRC_LIST})
source_group("prev/window" FILES ${WINDOW_SRC_LIST})
source_group("prev/window/impl" FILES ${WINDOW_IMPL_SRC_LIST})
source_group("prev/window/impl/windows" FILES ${WINDOW_IMPL_WINDOWS_SRC_LIST})
source_group("prev/window/impl/linux" FILES ${WINDOW_IMPL_LINUX_SRC_LIST})
source_group("prev/window/impl/android" FILES ${WINDOW_IMPL_ANDROID_SRC_LIST})

include_directories("../Dependencies/glm")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")  #for find_package

# Options to set
option(ENABLE_VALIDATION "Enable Vulkan Validation." ON) #adds 5KB to exe size
option(ENABLE_LOGGING "Prints LOG* messages to Terminal or Android Logcat." ON)
option(ENABLE_MULTITOUCH "Multi-touch screen support" OFF)

if (ENABLE_VALIDATION)
    add_definitions(-DENABLE_VALIDATION)
endif()
if (ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
endif()
if (ENABLE_MULTITOUCH)
    add_definitions(-DENABLE_MULTITOUCH)
endif()

add_definitions(-DVK_NO_PROTOTYPES)

if(WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

    add_library(${PROJECT_NAME} STATIC 
        ${ROOT_SRC_LIST}
        ${COMMON_SRC_LIST}
        ${COMMON_PATTERN_SRC_LIST}
        ${CORE_SRC_LIST}
        ${CORE_MEMORY_SRC_LIST}
        ${CORE_DEVICE_SRC_LIST}
        ${CORE_INSTANCE_SRC_LIST}
        ${EVENT_SRC_LIST}
        ${EVENT_INTERNAL_SRC_LIST}
        ${INPUT_SRC_LIST}
        ${INPUT_MOUSE_SRC_LIST}
        ${INPUT_KEYBOARD_SRC_LIST}
        ${INPUT_TOUCH_SRC_LIST}
        ${RENDER_SRC_LIST}
        ${RENDER_BUFFER_SRC_LIST}
        ${RENDER_BUFFER_IMAGE_SRC_LIST}
        ${RENDER_IMAGE_SRC_LIST}
        ${RENDER_PASS_SRC_LIST}
        ${RENDER_SHADER_SRC_LIST}
        ${RENDER_SAMPLER_SRC_LIST}
        ${SCENE_SRC_LIST}
        ${SCENE_COMPONENT_SRC_LIST}
        ${SCENE_GRAPH_SRC_LIST}
        ${UTIL_SRC_LIST}
        ${WINDOW_SRC_LIST}
        ${WINDOW_IMPL_SRC_LIST}
        ${WINDOW_IMPL_WINDOWS_SRC_LIST}
        ${WINDOW_IMPL_LINUX_SRC_LIST}
        ${WINDOW_IMPL_ANDROID_SRC_LIST}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    find_path(VULKAN_INCLUDE vulkan/vulkan.h PATH "$ENV{VULKAN_SDK}/include")    # Search in Vulkan SDK
    target_include_directories(${PROJECT_NAME} PUBLIC "${VULKAN_INCLUDE}")       # include vulkan.h
    message("VULKAN_INCLUDE path: ${VULKAN_INCLUDE}")

endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    set(BUILD_PREVENGINE_XCB_SUPPORT ON)  #Other options are not yet supported.
    #option(BUILD_PREVENGINE_XCB_SUPPORT     "Build XCB PREVENGINE support"      ON)
    #option(BUILD_PREVENGINE_XLIB_SUPPORT    "Build Xlib PREVENGINE support"    OFF)
    #option(BUILD_PREVENGINE_WAYLAND_SUPPORT "Build Wayland PREVENGINE support" OFF)
    #option(BUILD_PREVENGINE_MIR_SUPPORT     "Build Mir PREVENGINE support"     OFF)

    add_library(${PROJECT_NAME} STATIC 
        ${ROOT_SRC_LIST}
        ${COMMON_SRC_LIST}
        ${COMMON_PATTERN_SRC_LIST}
        ${CORE_SRC_LIST}
        ${CORE_MEMORY_SRC_LIST}
        ${CORE_DEVICE_SRC_LIST}
        ${CORE_INSTANCE_SRC_LIST}
        ${EVENT_SRC_LIST}
        ${EVENT_INTERNAL_SRC_LIST}
        ${INPUT_SRC_LIST}
        ${INPUT_MOUSE_SRC_LIST}
        ${INPUT_KEYBOARD_SRC_LIST}
        ${INPUT_TOUCH_SRC_LIST}
        ${RENDER_SRC_LIST}
        ${RENDER_BUFFER_SRC_LIST}
        ${RENDER_BUFFER_IMAGE_SRC_LIST}
        ${RENDER_IMAGE_SRC_LIST}
        ${RENDER_PASS_SRC_LIST}
        ${RENDER_SHADER_SRC_LIST}
        ${RENDER_SAMPLER_SRC_LIST}
        ${SCENE_SRC_LIST}
        ${SCENE_COMPONENT_SRC_LIST}
        ${SCENE_GRAPH_SRC_LIST}
        ${UTIL_SRC_LIST}
        ${WINDOW_SRC_LIST}
        ${WINDOW_IMPL_SRC_LIST}
        ${WINDOW_IMPL_WINDOWS_SRC_LIST}
        ${WINDOW_IMPL_LINUX_SRC_LIST}
        ${WINDOW_IMPL_ANDROID_SRC_LIST}
    )

    target_link_libraries(${PROJECT_NAME} pthread)

    if (BUILD_PREVENGINE_XCB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
        #---XCB---  (window)
        find_package(XCB REQUIRED)
        include_directories(${XCB_INCLUDE_DIRS})                 # /usr/include
        target_link_libraries(${PROJECT_NAME} ${XCB_LIBRARIES})  # /usr/lib/x86_64-linux-gnu/libxcb.so
        #---XKB--- (keyboard)
        find_library(XKB "xkbcommon" DOC "XKB Keyboard library") # xkb keyboard support
        target_link_libraries(${PROJECT_NAME} ${XKB})            # /usr/lib/x86_64-linux-gnu/libxkbcommon.so

        #---X11---
        find_package(X11 REQUIRED)
        include_directories(${X11_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
        #---X11-XCB---
        find_package(X11_XCB REQUIRED)
        include_directories(${X11_XCB_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${X11_XCB_LIBRARIES})
        #---XInput--- (MultiTouch)
        if (ENABLE_MULTITOUCH)               #adds 8.5KB to exe size
            target_link_libraries(${PROJECT_NAME} ${X11_Xinput_LIB})
            add_definitions(-DENABLE_MULTITOUCH)
        endif()
    endif()

#    if (BUILD_PREVENGINE_XLIB_SUPPORT)
#        add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
#    endif()

#    if (BUILD_PREVENGINE_WAYLAND_SUPPORT)
#        add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR)
#    endif()

#    if (BUILD_PREVENGINE_MIR_SUPPORT)
#        add_definitions(-DVK_USE_PLATFORM_MIR_KHR)
#    endif()

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    find_path(VULKAN_INCLUDE vulkan/vulkan.h PATH "$ENV{VULKAN_SDK}/include")    # Search in Vulkan SDK
    target_include_directories(${PROJECT_NAME} PUBLIC "${VULKAN_INCLUDE}")       # include vulkan.h
    message("VULKAN_INCLUDE path: ${VULKAN_INCLUDE}")
endif()

if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O3")

    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
    add_definitions(-DENABLE_VALIDATION)
    add_definitions(-DENABLE_LOGGING)
    add_definitions(-DGLM_FORCE_CXX17)
    add_definitions(-DRENDER_SELECTION)

    include_directories("android")
    include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

    file(GLOB ANDROID_SRC_LIST "android/*.h" "android/*.c" "android/*.cpp")
    file(GLOB ANDROID_NATIVE_GLUE_SRC_LIST "${ANDROID_NDK}/sources/android/native_app_glue/*.h" "${ANDROID_NDK}/sources/android/native_app_glue/*.c" "${ANDROID_NDK}/sources/android/native_app_glue/*.cpp")

    source_group("android" FILES ${ANDROID_SRC_LIST})
    source_group("${ANDROID_NDK}/sources/android/native_app_glue" ${ANDROID_NATIVE_GLUE_SRC_LIST})

    add_library(${PROJECT_NAME} STATIC 
        ${ROOT_SRC_LIST}
        ${COMMON_SRC_LIST}
        ${COMMON_PATTERN_SRC_LIST}
        ${CORE_SRC_LIST}
        ${CORE_MEMORY_SRC_LIST}
        ${CORE_DEVICE_SRC_LIST}
        ${CORE_INSTANCE_SRC_LIST}
        ${EVENT_SRC_LIST}
        ${EVENT_INTERNAL_SRC_LIST}
        ${INPUT_SRC_LIST}
        ${INPUT_MOUSE_SRC_LIST}
        ${INPUT_KEYBOARD_SRC_LIST}
        ${INPUT_TOUCH_SRC_LIST}
        ${RENDER_SRC_LIST}
        ${RENDER_BUFFER_SRC_LIST}
        ${RENDER_BUFFER_IMAGE_SRC_LIST}
        ${RENDER_IMAGE_SRC_LIST}
        ${RENDER_PASS_SRC_LIST}
        ${RENDER_SHADER_SRC_LIST}
        ${RENDER_SAMPLER_SRC_LIST}
        ${SCENE_SRC_LIST}
        ${SCENE_COMPONENT_SRC_LIST}
        ${SCENE_GRAPH_SRC_LIST}
        ${UTIL_SRC_LIST}
        ${WINDOW_SRC_LIST}
        ${WINDOW_IMPL_SRC_LIST}
        ${WINDOW_IMPL_WINDOWS_SRC_LIST}
        ${WINDOW_IMPL_LINUX_SRC_LIST}
        ${WINDOW_IMPL_ANDROID_SRC_LIST}
        ${ANDROID_SRC_LIST}
        ${ANDROID_NATIVE_GLUE_SRC_LIST}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()