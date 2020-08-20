cmake_minimum_required(VERSION 2.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

set(LIBRARY_NAME PreVEngine)

FILE(GLOB ROOT_SRC_LIST "*.h" "*.cpp")
FILE(GLOB EXT_SRC_LIST "external/*.h" "external/*.cpp")
FILE(GLOB COMMON_SRC_LIST "common/*.h" "common/*.cpp")
FILE(GLOB COMMON_PATTERN_SRC_LIST "common/pattern/*.h" "common/pattern/*.cpp")
FILE(GLOB CORE_SRC_LIST "core/*.h" "core/*.cpp")
FILE(GLOB CORE_MEMORY_SRC_LIST "core/memory/*.h" "core/memory/*.cpp")
FILE(GLOB CORE_MEMORY_BUFFER_SRC_LIST "core/memory/buffer/*.h" "core/memory/buffer/*.cpp")
FILE(GLOB CORE_MEMORY_IMAGE_SRC_LIST "core/memory/image/*.h" "core/memory/image/*.cpp")
FILE(GLOB CORE_DEVICE_SRC_LIST "core/device/*.h" "core/device/*.cpp")
FILE(GLOB CORE_INSTANCE_SRC_LIST "core/instance/*.h" "core/instance/*.cpp")
FILE(GLOB EVENT_SRC_LIST "event/*.h" "event/*.cpp")
FILE(GLOB EVENT_INTERNAL_SRC_LIST "event/internal/*.h" "event/internal/*.cpp")
FILE(GLOB INPUT_SRC_LIST "input/*.h" "input/*.cpp")
FILE(GLOB INPUT_MOUSE_SRC_LIST "input/mouse/*.h" "input/mouse/*.cpp")
FILE(GLOB INPUT_KEYBOARD_SRC_LIST "input/keyboard/*.h" "input/keyboard/*.cpp")
FILE(GLOB INPUT_TOUCH_SRC_LIST "input/touch/*.h" "input/touch/*.cpp")
FILE(GLOB RENDER_SRC_LIST "render/*.h" "render/*.cpp")
FILE(GLOB RENDER_IMAGE_SRC_LIST "render/image/*.h" "render/image/*.cpp")
FILE(GLOB RENDER_PASS_SRC_LIST "render/pass/*.h" "render/pass/*.cpp")
FILE(GLOB RENDER_SHADER_SRC_LIST "render/shader/*.h" "render/shader/*.cpp")
FILE(GLOB SCENE_SRC_LIST "scene/*.h" "scene/*.cpp")
FILE(GLOB SCENE_COMPONENT_SRC_LIST "scene/component/*.h" "scene/component/*.cpp")
FILE(GLOB SCENE_GRAPH_SRC_LIST "scene/graph/*.h" "scene/graph/*.cpp")
FILE(GLOB UTIL_SRC_LIST "util/*.h" "util/*.cpp")
FILE(GLOB WINDOW_SRC_LIST "window/*.h" "window/*.cpp")
FILE(GLOB WINDOW_IMPL_SRC_LIST "window/impl/*.h" "window/impl/*.cpp")
FILE(GLOB WINDOW_IMPL_WINDOWS_SRC_LIST "window/impl/windows/*.h" "window/impl/windows/*.cpp")
FILE(GLOB WINDOW_IMPL_LINUX_SRC_LIST "window/impl/linux/*.h" "window/impl/linux/*.cpp")
FILE(GLOB WINDOW_IMPL_ANDROID_SRC_LIST "window/impl/android/*.h" "window/impl/android/*.cpp")

SOURCE_GROUP("" FILES ${ROOT_SRC_LIST})
SOURCE_GROUP(external FILES ${EXT_SRC_LIST})
SOURCE_GROUP(common FILES ${COMMON_SRC_LIST})
SOURCE_GROUP(common/pattern FILES ${COMMON_PATTERN_SRC_LIST})
SOURCE_GROUP(core FILES ${CORE_SRC_LIST})
SOURCE_GROUP(core/memory FILES ${CORE_MEMORY_SRC_LIST})
SOURCE_GROUP(core/memory/buffer FILES ${CORE_MEMORY_BUFFER_SRC_LIST})
SOURCE_GROUP(core/memory/image FILES ${CORE_MEMORY_IMAGE_SRC_LIST})
SOURCE_GROUP(core/device FILES ${CORE_DEVICE_SRC_LIST})
SOURCE_GROUP(core/instance FILES ${CORE_INSTANCE_SRC_LIST})
SOURCE_GROUP(event FILES ${EVENT_SRC_LIST})
SOURCE_GROUP(event/internal FILES ${EVENT_INTERNAL_SRC_LIST})
SOURCE_GROUP(input FILES ${INPUT_SRC_LIST})
SOURCE_GROUP(input/mouse FILES ${INPUT_MOUSE_SRC_LIST})
SOURCE_GROUP(input/keyboard FILES ${INPUT_KEYBOARD_SRC_LIST})
SOURCE_GROUP(input/touch FILES ${INPUT_TOUCH_SRC_LIST})
SOURCE_GROUP(render FILES ${RENDER_SRC_LIST})
SOURCE_GROUP(render/image FILES ${RENDER_IMAGE_SRC_LIST})
SOURCE_GROUP(render/pass FILES ${RENDER_PASS_SRC_LIST})
SOURCE_GROUP(render/shader FILES ${RENDER_SHADER_SRC_LIST})
SOURCE_GROUP(scene FILES ${SCENE_SRC_LIST})
SOURCE_GROUP(scene/component FILES ${SCENE_COMPONENT_SRC_LIST})
SOURCE_GROUP(scene/graph FILES ${SCENE_GRAPH_SRC_LIST})
SOURCE_GROUP(util FILES ${UTIL_SRC_LIST})
SOURCE_GROUP(window FILES ${WINDOW_SRC_LIST})
SOURCE_GROUP(window/impl FILES ${WINDOW_IMPL_SRC_LIST})
SOURCE_GROUP(window/impl/windows FILES ${WINDOW_IMPL_WINDOWS_SRC_LIST})
SOURCE_GROUP(window/impl/linux FILES ${WINDOW_IMPL_LINUX_SRC_LIST})
SOURCE_GROUP(window/impl/android FILES ${WINDOW_IMPL_ANDROID_SRC_LIST})

add_library(${LIBRARY_NAME} STATIC 
    ${ROOT_SRC_LIST}
    ${EXT_SRC_LIST}
    ${COMMON_SRC_LIST}
    ${COMMON_PATTERN_SRC_LIST}
    ${CORE_SRC_LIST}
    ${CORE_MEMORY_SRC_LIST}
    ${CORE_MEMORY_BUFFER_SRC_LIST}
    ${CORE_MEMORY_IMAGE_SRC_LIST}
    ${CORE_DEVICE_SRC_LIST}
    ${CORE_INSTANCE_SRC_LIST}
    ${EVENT_SRC_LIST}
    ${EVENT_INTERNAL_SRC_LIST}
    ${INPUT_SRC_LIST}
    ${INPUT_MOUSE_SRC_LIST}
    ${INPUT_KEYBOARD_SRC_LIST}
    ${INPUT_TOUCH_SRC_LIST}
    ${RENDER_SRC_LIST}
    ${RENDER_IMAGE_SRC_LIST}
    ${RENDER_PASS_SRC_LIST}
    ${RENDER_SHADER_SRC_LIST}
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

target_include_directories (${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories("../Dependencies/glm")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")  #for find_package

#=========================VULKAN_WRAPPER==========================
if (USE_VULKAN_WRAPPER)
    add_subdirectory(vk_wrapper)
    target_link_libraries(${LIBRARY_NAME} VULKAN_WRAPPER)
endif()
#=================================================================
#============================VALIDATION===========================
#  Validation may be enabled in both debug and release builds.
#  Logging is enabled in debug builds, or when validation is enabled.

option(ENABLE_VALIDATION "Enable Vulkan Validation."  ON) #adds 5KB to exe size
if (ENABLE_VALIDATION)
    add_definitions(-DENABLE_VALIDATION)
endif()

option(ENABLE_LOGGING "Prints LOG* messages to Terminal or Android Logcat." ON)
if (ENABLE_LOGGING)
    target_compile_definitions(${LIBRARY_NAME} PUBLIC -DENABLE_LOGGING) #enable logging for target project
endif()
#=================================================================
#============================MULTI-TOUCH==========================
option(ENABLE_MULTITOUCH "Multi-touch screen support" OFF)
if (ENABLE_MULTITOUCH)
    add_definitions(-DENABLE_MULTITOUCH)
endif()
#=================================================================
#==============================LINUX==============================
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-std=c++17)
    set(BUILD_PREVENGINE_XCB_SUPPORT ON)  #Other options are not yet supported.
    #option(BUILD_PREVENGINE_XCB_SUPPORT     "Build XCB PREVENGINE support"      ON)
    #option(BUILD_PREVENGINE_XLIB_SUPPORT    "Build Xlib PREVENGINE support"    OFF)
    #option(BUILD_PREVENGINE_WAYLAND_SUPPORT "Build Wayland PREVENGINE support" OFF)
    #option(BUILD_PREVENGINE_MIR_SUPPORT     "Build Mir PREVENGINE support"     OFF)

    target_link_libraries(${LIBRARY_NAME} pthread)

    if (BUILD_PREVENGINE_XCB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
        #---XCB---  (window)
        find_package(XCB REQUIRED)
        include_directories(${XCB_INCLUDE_DIRS})                 # /usr/include
        target_link_libraries(${LIBRARY_NAME} ${XCB_LIBRARIES})  # /usr/lib/x86_64-linux-gnu/libxcb.so
        #---XKB--- (keyboard)
        find_library(XKB "xkbcommon" DOC "XKB Keyboard library") # xkb keyboard support
        target_link_libraries(${LIBRARY_NAME} ${XKB})            # /usr/lib/x86_64-linux-gnu/libxkbcommon.so

        #---X11---
        find_package(X11 REQUIRED)
        include_directories(${X11_INCLUDE_DIR})
        target_link_libraries(${LIBRARY_NAME} ${X11_LIBRARIES})
        #---X11-XCB---
        find_package(X11_XCB REQUIRED)
        include_directories(${X11_XCB_INCLUDE_DIR})
        target_link_libraries(${LIBRARY_NAME} ${X11_XCB_LIBRARIES})
        #---XInput--- (MultiTouch)
        if (ENABLE_MULTITOUCH)               #adds 8.5KB to exe size
            target_link_libraries(${LIBRARY_NAME} ${X11_Xinput_LIB})
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

    set (VULKAN_LOADER_NAME "vulkan")
endif()
#=================================================================
#=============================WINDOWS=============================
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
    set (VULKAN_LOADER_NAME "vulkan-${MAJOR}")
endif()
#=================================================================
#=============================ANDROID=============================
# Note: Android has its own CMakeLists.txt in the Android/app folder.
#if(ANDROID)
#    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
#endif()
#=================================================================
#=======================Find Vulkan Loader========================
if(TARGET ${VULKAN_LOADER_NAME})                                                 # Check if "vulkan" loader target exists
    target_link_libraries(${LIBRARY_NAME} ${VULKAN_LOADER_NAME})                 # Set dependency and link to loader target
else()                                                                           # If loader is not being built here,
    find_library(VULKAN_LOADER                                                   # then search for a pre-built one.
        NAMES "vulkan" "vulkan-{MAJOR}" "vulkan-1"
        HINTS "${CMAKE_SOURCE_DIR}/../${BINDATA_DIR}"                            # For SDK build?
              "${CMAKE_SOURCE_DIR}/../x86_64/lib"                                # for SDK build?
              "../../../build/${CMAKE_BUILD_TYPE}/loader"                        # Search in VulkanSamples
              "$ENV{VULKAN_SDK}/lib"                                             # Search in Vulkan SDK on Linux  : /lib/libvulkan.so
              "$ENV{VULKAN_SDK}/bin"                                             # Search in Vulkan SDK on Windows: /bin/vulkan-1.lib
        DOC "Path + Filename of Vulkan loader. (libvulkan.so or vulkan-1.lib)")  # MUST INCLUDE FILENAME
    if(EXISTS ${VULKAN_LOADER})
        target_link_libraries(${LIBRARY_NAME} ${VULKAN_LOADER})                  # Link to loader
    endif()
    message("VULKAN_LOADER path: ${VULKAN_LOADER}")
endif()
#=================================================================
#=======================Find Vulkan Header========================
find_path(VULKAN_INCLUDE vulkan/vulkan.h DOC "Path to Vulkan header file. (vulkan.h)")
find_path(VULKAN_INCLUDE vulkan/vulkan.h PATH "$ENV{VULKAN_SDK}/include")    # Search in Vulkan SDK
find_path(VULKAN_INCLUDE vulkan/vulkan.h PATH "../../include")               # Search in VulkanSamples
find_path(VULKAN_INCLUDE vulkan/vulkan.h PATH "vk_wrapper")                  # Search in vk_wrapper
target_include_directories(${LIBRARY_NAME} PUBLIC "${VULKAN_INCLUDE}")       # include vulkan.h
message("VULKAN_INCLUDE path: ${VULKAN_INCLUDE}")
#=================================================================
