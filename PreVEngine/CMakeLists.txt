cmake_minimum_required(VERSION 3.10)

project(PreVEngine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_LIST 
    "prev/*.h" "prev/*.cpp"
    "prev/common/*.h" "prev/common/*.cpp"
    "prev/common/pattern/*.h" "prev/common/pattern/*.cpp"
    "prev/core/*.h" "prev/core/*.cpp"
    "prev/core/memory/*.h" "prev/core/memory/*.cpp"
    "prev/core/device/*.h" "prev/core/device/*.cpp"
    "prev/core/instance/*.h" "prev/core/instance/*.cpp"
    "prev/event/*.h" "prev/event/*.cpp"
    "prev/input/*.h" "prev/input/*.cpp"
    "prev/input/mouse/*.h" "prev/input/mouse/*.cpp"
    "prev/input/keyboard/*.h" "prev/input/keyboard/*.cpp"
    "prev/input/touch/*.h" "prev/input/touch/*.cpp"
    "prev/render/*.h" "prev/render/*.cpp"
    "prev/render/buffer/*.h" "prev/render/buffer/*.cpp"
    "prev/render/image/*.h" "prev/render/image/*.cpp"
    "prev/render/pass/*.h" "prev/render/pass/*.cpp"
    "prev/render/shader/*.h" "prev/render/shader/*.cpp"
    "prev/render/sampler/*.h" "prev/render/sampler/*.cpp"
    "prev/render/pipeline/*.h" "prev/render/pipeline/*.cpp"
    "prev/render/query/*.h" "prev/render/query/*.cpp"
    "prev/scene/*.h" "prev/scene/*.cpp"
    "prev/scene/component/*.h" "prev/scene/component/*.cpp"
    "prev/scene/graph/*.h" "prev/scene/graph/*.cpp"
    "prev/util/*.h" "prev/util/*.cpp"
    "prev/window/*.h" "prev/window/*.cpp"
    "prev/window/impl/*.h" "prev/window/impl/*.cpp"
    "prev/window/impl/windows/*.h" "prev/window/impl/windows/*.cpp"
    "prev/window/impl/xcb/*.h" "prev/window/impl/xcb/*.cpp"
    "prev/window/impl/android/*.h" "prev/window/impl/android/*.cpp"
    "prev/window/impl/macos/*.h" "prev/window/impl/macos/*.cpp"
    "prev/window/impl/ios/*.h" "prev/window/impl/ios/*.cpp"
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")  # for find_package

# Options to set
option(ENABLE_LOGGING "Prints LOG* messages to Terminal or Android Logcat." ON)
option(ENABLE_MULTITOUCH "Multi-touch screen support" OFF)
option(ENABLE_REVERSE_DEPTH "Enable reverse depth" OFF)
option(ENABLE_VK_LOADER "Enable Vulkan loader" ON)

if (ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
endif()
if (ENABLE_MULTITOUCH)
    add_definitions(-DENABLE_MULTITOUCH)
endif()
if (ENABLE_REVERSE_DEPTH)
    add_definitions(-DENABLE_REVERSE_DEPTH)
endif()
if (ENABLE_VK_LOADER)
    add_definitions(-DENABLE_VK_LOADER)
endif()

if(ENABLE_VK_LOADER)
    add_definitions(-DVK_NO_PROTOTYPES)
endif()

include_directories("external/worm_holes/WormHoles")
include_directories("external/glm")

if(WIN32)
    message("${PROJECT_NAME} - Windows")
    add_definitions(-DTARGET_PLATFORM_WINDOWS)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

    add_library(${PROJECT_NAME} STATIC ${SRC_LIST})

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME} PUBLIC "$ENV{VULKAN_SDK}/include")

elseif(IOS)
    message("${PROJECT_NAME} - iOS")
    add_definitions(-DTARGET_PLATFORM_IOS)
    add_definitions(-DVK_USE_PLATFORM_IOS_MVK)

    include_directories("platform/ios")

    file(GLOB IOS_SRC_LIST 
        "platform/ios/*.h" "platform/ios/*.mm"
        "prev/window/impl/ios/*.h" "prev/window/impl/ios/*.mm"
    )
    
    find_library(UIKIT_LIB UIKit)
    find_library(METALKIT_LIB METALKit)
    find_library(FOUNDATION_LIB Foundation)
  
    add_library(${PROJECT_NAME} STATIC
        ${SRC_LIST}
        ${IOS_SRC_LIST}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME} PUBLIC "$ENV{VULKAN_SDK}/include")

    target_link_libraries(${PROJECT_NAME} 
        ${UIKIT_LIB}
        ${METALKIT_LIB}
        ${FOUNDATION_LIB}
    )
    
elseif(APPLE)
    message("${PROJECT_NAME} - MacOS")
    add_definitions(-DTARGET_PLATFORM_MACOS)
    add_definitions(-DVK_USE_PLATFORM_MACOS_MVK)

    include_directories("platform/macos")

    file(GLOB MACOS_SRC_LIST 
        "platform/macos/*.h" "platform/macos/*.mm"
        "prev/window/impl/macos/*.h" "prev/window/impl/macos/*.mm"
    )
    
    find_library(COCOA_LIB Cocoa)
    find_library(QUARTZ_CORE_LIB QuartzCore)

    add_library(${PROJECT_NAME} STATIC
        ${SRC_LIST}
        ${MACOS_SRC_LIST}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME} PUBLIC "$ENV{VULKAN_SDK}/include")

    target_link_libraries(${PROJECT_NAME} 
        ${COCOA_LIB}
        ${QUARTZ_CORE_LIB}
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    message("${PROJECT_NAME} - Linux")
    add_definitions(-DTARGET_PLATFORM_LINUX)

    option(BUILD_PREVENGINE_XCB_SUPPORT "Build XCB PREVENGINE support" ON)
    #option(BUILD_PREVENGINE_XLIB_SUPPORT "Build Xlib PREVENGINE support" OFF)
    #option(BUILD_PREVENGINE_WAYLAND_SUPPORT "Build Wayland PREVENGINE support" OFF)
    #option(BUILD_PREVENGINE_MIR_SUPPORT "Build Mir PREVENGINE support" OFF)

    add_library(${PROJECT_NAME} STATIC ${SRC_LIST})

    target_link_libraries(${PROJECT_NAME} pthread)

    if (BUILD_PREVENGINE_XCB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
        #---XCB---  (window)
        find_package(XCB REQUIRED)
        include_directories(${XCB_INCLUDE_DIRS})                 # /usr/include
        target_link_libraries(${PROJECT_NAME} ${XCB_LIBRARIES})  # /usr/lib/x86_64-linux-gnu/libxcb.so
        #---XKB--- (keyboard)
        # find_package(XKB REQUIRED)
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
    target_include_directories(${PROJECT_NAME} PUBLIC "$ENV{VULKAN_SDK}/include")

elseif(ANDROID)
    message("${PROJECT_NAME} - Android")
    add_definitions(-DTARGET_PLATFORM_ANDROID)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O3")

    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
    add_definitions(-DGLM_FORCE_CXX17)

    include_directories("platform/android")
    include_directories("${ANDROID_NDK}/sources/android/native_app_glue")

    file(GLOB ANDROID_SRC_LIST "platform/android/*.h" "platform/android/*.c" "platform/android/*.cpp")
    file(GLOB ANDROID_NATIVE_GLUE_SRC_LIST "${ANDROID_NDK}/sources/android/native_app_glue/*.h" "${ANDROID_NDK}/sources/android/native_app_glue/*.c" "${ANDROID_NDK}/sources/android/native_app_glue/*.cpp")

    add_library(${PROJECT_NAME} STATIC 
        ${SRC_LIST}
        ${ANDROID_SRC_LIST}
        ${ANDROID_NATIVE_GLUE_SRC_LIST}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
else()
    message(FATAL_ERROR "Unsupported plaform.")
endif()
