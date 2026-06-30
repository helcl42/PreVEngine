cmake_minimum_required(VERSION 3.16)

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
    "prev/core/engine/*.h" "prev/core/engine/*.cpp"
    "prev/core/engine/impl/*.h" "prev/core/engine/impl/*.cpp"
    "prev/core/memory/*.h" "prev/core/memory/*.cpp"
    "prev/core/device/*.h" "prev/core/device/*.cpp"
    "prev/core/instance/*.h" "prev/core/instance/*.cpp"
    "prev/core/sync/*.h" "prev/core/sync/*.cpp"
    "prev/event/*.h" "prev/event/*.cpp"
    "prev/input/*.h" "prev/input/*.cpp"
    "prev/input/mouse/*.h" "prev/input/mouse/*.cpp"
    "prev/input/keyboard/*.h" "prev/input/keyboard/*.cpp"
    "prev/input/touch/*.h" "prev/input/touch/*.cpp"
    "prev/render/*.h" "prev/render/*.cpp"
    "prev/render/buffer/*.h" "prev/render/buffer/*.cpp"
    "prev/render/framebuffer/*.h" "prev/render/framebuffer/*.cpp"
    "prev/render/image/*.h" "prev/render/image/*.cpp"
    "prev/render/pass/*.h" "prev/render/pass/*.cpp"
    "prev/render/shader/*.h" "prev/render/shader/*.cpp"
    "prev/render/sampler/*.h" "prev/render/sampler/*.cpp"
    "prev/render/pipeline/*.h" "prev/render/pipeline/*.cpp"
    "prev/render/query/*.h" "prev/render/query/*.cpp"
    "prev/render/surface/*.h" "prev/render/surface/*.cpp"
    "prev/render/swapchain/*.h" "prev/render/swapchain/*.cpp"
    "prev/render/swapchain/presentable/*.h" "prev/render/swapchain/presentable/*.cpp"
    "prev/render/swapchain/headless/*.h" "prev/render/swapchain/headless/*.cpp"
    "prev/scene/*.h" "prev/scene/*.cpp"
    "prev/scene/component/*.h" "prev/scene/component/*.cpp"
    "prev/scene/graph/*.h" "prev/scene/graph/*.cpp"
    "prev/util/*.h" "prev/util/*.cpp"
    "prev/util/intersection/*.h" "prev/util/intersection/*.cpp"
    "prev/window/*.h" "prev/window/*.cpp"
    "prev/window/impl/*.h" "prev/window/impl/*.cpp"
    "prev/window/impl/windows/*.h" "prev/window/impl/windows/*.cpp"
    "prev/window/impl/xcb/*.h" "prev/window/impl/xcb/*.cpp"
    "prev/window/impl/wayland/*.h" "prev/window/impl/wayland/*.cpp"
    "prev/window/impl/android/*.h" "prev/window/impl/android/*.cpp"
    "prev/window/impl/macos/*.h" "prev/window/impl/macos/*.cpp"
    "prev/window/impl/ios/*.h" "prev/window/impl/ios/*.cpp"
    "prev/window/impl/headless/*.h" "prev/window/impl/headless/*.cpp"
    "prev/window/impl/emscripten/*.h" "prev/window/impl/emscripten/*.cpp"
    "prev/xr/*.h" "prev/xr/*.cpp"
    "prev/xr/open_xr/*.h" "prev/xr/open_xr/*.cpp"
    "prev/xr/open_xr/common/*.h" "prev/xr/open_xr/common/*.cpp"
    "prev/xr/open_xr/core/*.h" "prev/xr/open_xr/core/*.cpp"
    "prev/xr/open_xr/render/*.h" "prev/xr/open_xr/render/*.cpp"
    "prev/xr/open_xr/input/*.h" "prev/xr/open_xr/input/*.cpp"
    "prev/xr/open_xr/input/util/*.h" "prev/xr/open_xr/input/util/*.cpp"
    "prev/xr/open_xr/util/*.h" "prev/xr/open_xr/util/*.cpp"
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")  # for find_package

# Options to set
option(ENABLE_LOGGING "Prints LOG* messages to Terminal or Android Logcat." ON)
option(ENABLE_MULTITOUCH "Multi-touch screen support" OFF)
option(ENABLE_REVERSE_DEPTH "Enable reverse depth" OFF)
option(ENABLE_XR "Enable XR" OFF)
option(ENABLE_XR_EXTENDED_INIT "Enable XR extended init (currently applicable on Android only)" OFF)
option(ENABLE_XR_DEPTH "Enable XR Depth" OFF)
option(BUILD_VULKAN_BACKEND "Build Vulkan backend" ON)
option(BUILD_WEBGPU_BACKEND "Build WebGPU backend (requires Dawn)" ON)
option(BUILD_HEADLESS "Build without windowing system support (no surface creation)" OFF)
option(ENABLE_ASAN  "Enable Address Sanitizer"            OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(ENABLE_TSAN  "Enable Thread Sanitizer"             OFF)

if (ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
endif()
if (ENABLE_MULTITOUCH)
    add_definitions(-DENABLE_MULTITOUCH)
endif()
if (ENABLE_REVERSE_DEPTH)
    add_definitions(-DENABLE_REVERSE_DEPTH)
endif()
if(ENABLE_XR)
    add_definitions(-DENABLE_XR)
    set(MAX_VIEW_COUNT 2 CACHE STRING "Maximum number of views for XR")
else()
    set(MAX_VIEW_COUNT 1 CACHE STRING "Maximum number of views (1 = mono)" FORCE)
endif()
if(ENABLE_XR_DEPTH)
    add_definitions(-DENABLE_XR_DEPTH)
endif()

include_directories("external")

# Emscripten forces WebGPU-only build
if(EMSCRIPTEN)
    set(BUILD_VULKAN_BACKEND OFF CACHE BOOL "Vulkan not available on web" FORCE)
    set(BUILD_WEBGPU_BACKEND ON CACHE BOOL "" FORCE)
    # Sanitizers are not supported on web - force off so the flags never reach the Emscripten link.
    set(ENABLE_ASAN  OFF CACHE BOOL "Sanitizers not supported on web" FORCE)
    set(ENABLE_UBSAN OFF CACHE BOOL "Sanitizers not supported on web" FORCE)
    set(ENABLE_TSAN  OFF CACHE BOOL "Sanitizers not supported on web" FORCE)
    add_definitions(-DWORM_HOLES_SINGLE_THREADED)
endif()

# ---------------------------------------------------------------------------
# Sanitizers (Clang/GCC; AddressSanitizer also on MSVC). The derivation lives
# here once and is carried by the prev_sanitizers INTERFACE target, which every
# first-party target links (PreVEngine, the examples, the tests). Vendored code
# under external/ is left uninstrumented - it is never linked against this target.
# ASan and TSan are mutually exclusive; UBSan combines with either.
# ---------------------------------------------------------------------------
if(ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "ENABLE_ASAN and ENABLE_TSAN are mutually exclusive")
endif()

add_library(prev_sanitizers INTERFACE)
if(MSVC)
    if(ENABLE_ASAN)
        target_compile_options(prev_sanitizers INTERFACE /fsanitize=address)
        message(STATUS "Sanitizers enabled: address (MSVC)")
    endif()
    if(ENABLE_UBSAN OR ENABLE_TSAN)
        message(WARNING "UBSan/TSan are not supported on MSVC - ignoring")
    endif()
else()
    set(_prev_sanitizers "")
    if(ENABLE_ASAN)
        list(APPEND _prev_sanitizers address)
    endif()
    if(ENABLE_UBSAN)
        list(APPEND _prev_sanitizers undefined)
    endif()
    if(ENABLE_TSAN)
        list(APPEND _prev_sanitizers thread)
    endif()
    if(_prev_sanitizers)
        string(REPLACE ";" "," _prev_sanitizers_flag "${_prev_sanitizers}")
        target_compile_options(prev_sanitizers INTERFACE -fsanitize=${_prev_sanitizers_flag} -fno-omit-frame-pointer -g)
        target_link_options(prev_sanitizers INTERFACE -fsanitize=${_prev_sanitizers_flag})
        message(STATUS "Sanitizers enabled: ${_prev_sanitizers_flag}")
    endif()
endif()

if(IOS OR ANDROID OR ENABLE_XR)
    set(BUILD_WEBGPU_BACKEND OFF CACHE BOOL "WebGPU not supported on mobile and XR" FORCE)
endIf()

# Add gfx as a subdirectory — it manages its own sources, backends, and Dawn
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_CPP_WRAPPER OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Propagate windowing system selection to gfx
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    if(ENABLE_WAYLAND AND NOT ENABLE_XCB)
        set(GFX_ENABLE_X11 OFF CACHE BOOL "" FORCE)
        set(GFX_ENABLE_XCB OFF CACHE BOOL "" FORCE)
    elseif(ENABLE_XCB AND NOT ENABLE_WAYLAND)
        set(GFX_ENABLE_WAYLAND OFF CACHE BOOL "" FORCE)
    endif()
endif()

add_subdirectory(external/gfx)

include_directories("external/worm_holes/WormHoles")
include_directories("external/glm")

# XR: Vulkan-only, fetch OpenXR loader
if(ENABLE_XR)
    set(BUILD_WEBGPU_BACKEND OFF CACHE BOOL "WebGPU not supported with XR" FORCE)

    include(FetchContent)
    set(DYNAMIC_LOADER OFF CACHE BOOL "Build OpenXR loader as static library" FORCE)
    set(BUILD_API_LAYERS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        openxr_sdk
        GIT_REPOSITORY https://github.com/KhronosGroup/OpenXR-SDK.git
        GIT_TAG        release-1.1.59
    )
    FetchContent_MakeAvailable(openxr_sdk)
endif()

if(WIN32)
    message("${PROJECT_NAME} - Windows")
    add_definitions(-DTARGET_PLATFORM_WINDOWS)
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    add_library(${PROJECT_NAME} STATIC ${SRC_LIST})

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${PROJECT_NAME} gfx)

elseif(IOS)
    message("${PROJECT_NAME} - iOS")
    add_definitions(-DTARGET_PLATFORM_IOS)

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

    target_link_libraries(${PROJECT_NAME} 
        gfx
        ${UIKIT_LIB}
        ${METALKIT_LIB}
        ${FOUNDATION_LIB}
    )
    
elseif(APPLE)
    message("${PROJECT_NAME} - MacOS")
    add_definitions(-DTARGET_PLATFORM_MACOS)

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

    target_link_libraries(${PROJECT_NAME} 
        gfx
        ${COCOA_LIB}
        ${QUARTZ_CORE_LIB}
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    message("${PROJECT_NAME} - Linux")
    add_definitions(-DTARGET_PLATFORM_LINUX)

    option(ENABLE_XCB "Enable XCB support" OFF)
    option(ENABLE_WAYLAND "Enable Wayland support" OFF)
    option(ENABLE_XLIB "Enable Xlib support" ON)
    
    if(BUILD_HEADLESS)
        message("${PROJECT_NAME} - Linux headless (no X11/XCB/Wayland)")

        add_library(${PROJECT_NAME} STATIC ${SRC_LIST})
        target_link_libraries(${PROJECT_NAME} gfx pthread)
    elseif (ENABLE_XCB)
        add_definitions(-DTARGET_PLATFORM_XCB)

        add_library(${PROJECT_NAME} STATIC ${SRC_LIST})

        target_link_libraries(${PROJECT_NAME} gfx pthread)

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
        #---Xlib native handle---
        if (ENABLE_XLIB)
            add_definitions(-DENABLE_XLIB_HANDLE)
        endif()
        #---XInput--- (MultiTouch)
        if (ENABLE_MULTITOUCH)               #adds 8.5KB to exe size
            target_link_libraries(${PROJECT_NAME} ${X11_Xinput_LIB})
            add_definitions(-DENABLE_MULTITOUCH)
        endif()
    elseif(ENABLE_WAYLAND)
        add_definitions(-DTARGET_PLATFORM_WAYLAND)

        include_directories("platform/wayland")

        file(GLOB WAYLAND_SRC_LIST 
            "platform/wayland/*.h" "platform/wayland/*.c"
        )
    
        add_library(${PROJECT_NAME} STATIC 
            ${SRC_LIST}
            ${WAYLAND_SRC_LIST}
        )

        target_link_libraries(${PROJECT_NAME} gfx)

        find_package(Wayland REQUIRED)
        include_directories(${WAYLAND_CLIENT_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${WAYLAND_CLIENT_LIBRARIES})
    else()
        message(FATAL_ERROR "Unsupported Linux Window System.")
    endif()

    # gfx auto-detects Wayland and publicly defines GFX_HAS_WAYLAND independently of PreVEngine's
    # window-system selection. WaylandWindowImpl.cpp is globbed into every Linux build and self-guards
    # on that macro, so it needs platform/wayland on the include path exactly when gfx defined it
    # (e.g. an XCB build that also has Wayland) - and never in a headless build, where gfx defines nothing.
    # gfx's GFX_HAS_WAYLAND is a directory-scoped variable not visible here, so read the actual gfx
    # target definition instead - the same condition that gates the WaylandWindowImpl source.
    get_target_property(_gfx_compile_defs gfx INTERFACE_COMPILE_DEFINITIONS)
    if(_gfx_compile_defs MATCHES "GFX_HAS_WAYLAND")
        include_directories("platform/wayland")
    endif()

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

elseif(ANDROID)
    message("${PROJECT_NAME} - Android")
    add_definitions(-DTARGET_PLATFORM_ANDROID)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O3")

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
    target_link_libraries(${PROJECT_NAME} gfx)

elseif(EMSCRIPTEN)
    message("${PROJECT_NAME} - Emscripten/Web")
    add_definitions(-DTARGET_PLATFORM_EMSCRIPTEN)

    add_library(${PROJECT_NAME} STATIC ${SRC_LIST})

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${PROJECT_NAME} gfx)
    target_compile_options(${PROJECT_NAME} PUBLIC "SHELL:-sDISABLE_EXCEPTION_CATCHING=0")
    target_link_options(${PROJECT_NAME} PUBLIC "SHELL:-sASYNCIFY=1" "SHELL:-sDISABLE_EXCEPTION_CATCHING=0")
else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

if(ENABLE_XR)
    message("${PROJECT_NAME} - Using XR")
    target_link_libraries(${PROJECT_NAME} openxr_loader)
    target_include_directories(${PROJECT_NAME} PUBLIC ${openxr_sdk_SOURCE_DIR}/include)
    if(ENABLE_XR_DEPTH)
        message("${PROJECT_NAME} - Using XR Depth")
    else()
        message("${PROJECT_NAME} - Not using XR Depth")
    endif()
else()
    message("${PROJECT_NAME} - Not using XR")
endif()

# Propagate MAX_VIEW_COUNT to all dependents
target_compile_definitions(${PROJECT_NAME} PUBLIC MAX_VIEW_COUNT_VALUE=${MAX_VIEW_COUNT})

# Sanitizers (see top-level CMakeLists). No-op unless ENABLE_ASAN/UBSAN/TSAN is set.
target_link_libraries(${PROJECT_NAME} prev_sanitizers)
