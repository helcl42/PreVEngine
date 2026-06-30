cmake_minimum_required(VERSION 3.10)

project(Sandbox)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_LIST
    "sandbox/*.h" "sandbox/*.cpp"
    "sandbox/common/*.h" "sandbox/common/*.cpp"
    "sandbox/component/*.h" "sandbox/component/*.cpp"
    "sandbox/scene/*.h" "sandbox/scene/*.cpp"
    "sandbox/scene/camera/*.h" "sandbox/scene/camera/*.cpp"
    "sandbox/render/*.h" "sandbox/render/*.cpp"
)
# XrCamera.{h,cpp} guard themselves with #ifdef ENABLE_XR (they reference prev::xr::*), so they can
# be globbed unconditionally: in mono builds they compile to an empty translation unit.

# Options derived from PreVEngine
if (ENABLE_REVERSE_DEPTH)
    add_definitions(-DENABLE_REVERSE_DEPTH)
endif()
if (ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
endif()
# PreVEngine scopes -DENABLE_XR to its own subdirectory, so the example must define it for its
# own sources (selects the stereo multiview camera/shader path). XR is Vulkan-only.
if (ENABLE_XR)
    add_definitions(-DENABLE_XR)
endif()

add_subdirectory(assets/Shaders)

include_directories("../../PreVEngine")
include_directories("../../PreVEngine/external")
include_directories("../../PreVEngine/external/worm_holes/WormHoles")
include_directories("../../PreVEngine/external/gfx/gfx/include")
include_directories("../../PreVEngine/external/glm")

if(WIN32)
    message("${PROJECT_NAME} - Windows")
    add_definitions(-DTARGET_PLATFORM_WINDOWS)
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    file(GLOB COMMON_SRC_LIST "platform/common/*.h" "platform/common/*.cpp")
    add_executable(${PROJECT_NAME} ${SRC_LIST} ${COMMON_SRC_LIST})
    target_link_libraries(${PROJECT_NAME} PreVEngine)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

elseif(IOS)
    message("${PROJECT_NAME} - iOS")
    add_definitions(-DTARGET_PLATFORM_IOS)

    set(APP_BUNDLE_IDENTIFIER "com.prevengine.Sandbox")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")

    set(IOS_LAUNCH_STORYBOARD "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/Launch Screen.storyboard")

    find_library(UIKIT_LIB UIKit)
    find_library(METALKIT_LIB MetalKit)
    find_library(FOUNDATION_LIB Foundation)

    file(GLOB IOS_SRC_LIST "platform/ios/*.h" "platform/ios/*.mm")

    # The iOS app entry point (IOSAppDelegate / IOSUtils) lives in the engine's iOS platform layer.
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../PreVEngine/platform/ios")

    add_executable(${PROJECT_NAME}
        ${SRC_LIST}
        ${IOS_SRC_LIST}
        ${IOS_LAUNCH_STORYBOARD}
    )

    target_link_libraries(${PROJECT_NAME} PreVEngine ${UIKIT_LIB} ${METALKIT_LIB} ${FOUNDATION_LIB})

    # MoltenVK from the Vulkan SDK (simulator slice; for device add the ios-arm64 slice similarly).
    if(${CMAKE_OSX_SYSROOT} MATCHES "iPhoneSimulator")
        target_link_libraries(${PROJECT_NAME} "-framework $ENV{VULKAN_SDK}/../iOS/lib/MoltenVK.xcframework/ios-arm64_x86_64-simulator/MoltenVK.framework/MoltenVK")
    endif()

    # Bundle assets as app resources, preserving directory structure (so ShaderPath resolves them).
    file(GLOB_RECURSE ASSET_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
    target_sources(${PROJECT_NAME} PRIVATE ${ASSET_FILES})
    foreach(ASSET_FILE ${ASSET_FILES})
        file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets" ${ASSET_FILE})
        get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
        if(REL_DIR)
            set_source_files_properties(${ASSET_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/${REL_DIR}" HEADER_FILE_ONLY TRUE)
        else()
            set_source_files_properties(${ASSET_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources" HEADER_FILE_ONLY TRUE)
        endif()
    endforeach()

    set_source_files_properties(${IOS_LAUNCH_STORYBOARD} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/Info.plist"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${APP_BUNDLE_IDENTIFIER}
    )

elseif(APPLE)
    message("${PROJECT_NAME} - macOS")
    add_definitions(-DTARGET_PLATFORM_MACOS)

    file(GLOB COMMON_SRC_LIST "platform/common/*.h" "platform/common/*.cpp")
    add_executable(${PROJECT_NAME} ${SRC_LIST} ${COMMON_SRC_LIST})
    target_link_libraries(${PROJECT_NAME} PreVEngine)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    message("${PROJECT_NAME} - Linux")
    add_definitions(-DTARGET_PLATFORM_LINUX)

    file(GLOB COMMON_SRC_LIST "platform/common/*.h" "platform/common/*.cpp")
    add_executable(${PROJECT_NAME} ${SRC_LIST} ${COMMON_SRC_LIST})
    target_link_libraries(${PROJECT_NAME} PreVEngine)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

elseif(EMSCRIPTEN)
    message("${PROJECT_NAME} - Emscripten/Web")
    add_definitions(-DTARGET_PLATFORM_EMSCRIPTEN)
    add_definitions(-DWORM_HOLES_SINGLE_THREADED)

    file(GLOB COMMON_SRC_LIST "platform/common/*.h" "platform/common/*.cpp")
    add_executable(${PROJECT_NAME} ${SRC_LIST} ${COMMON_SRC_LIST})
    target_link_libraries(${PROJECT_NAME} PreVEngine)

    target_compile_options(${PROJECT_NAME} PRIVATE "SHELL:-sDISABLE_EXCEPTION_CATCHING=0" -g2)
    target_link_options(${PROJECT_NAME} PRIVATE
        "SHELL:--use-port=emdawnwebgpu"
        "SHELL:-sALLOW_MEMORY_GROWTH=1"
        "SHELL:-sASYNCIFY=1"
        "SHELL:-sASYNCIFY_STACK_SIZE=65536"
        "SHELL:-sSTACK_SIZE=1048576"
        "SHELL:-sASSERTIONS=2"
        "SHELL:-sDISABLE_EXCEPTION_CATCHING=0"
        "SHELL:-sEXCEPTION_STACK_TRACES=1"
        -g2
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@assets"
        "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/platform/web/shell.html"
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

elseif(ANDROID)
    message("${PROJECT_NAME} - Android")
    add_definitions(-DTARGET_PLATFORM_ANDROID)
    add_definitions(-DGLM_FORCE_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O3")

    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../PreVEngine/platform/android")
    include_directories("${ANDROID_NDK}/sources/android/native_app_glue")

    file(GLOB COMMON_SRC_LIST "platform/android/app/src/main/cpp/*.h" "platform/android/app/src/main/cpp/*.cpp")
    # NativeActivity loads the shared library named in AndroidManifest (android.app.lib_name = "Sandbox").
    add_library(${PROJECT_NAME} SHARED ${SRC_LIST} ${COMMON_SRC_LIST})
    target_link_libraries(${PROJECT_NAME} PreVEngine log android)

else()
    message(FATAL_ERROR "Unsupported platform for Sandbox example.")
endif()

# Sanitizers (see top-level CMakeLists). No-op unless ENABLE_ASAN/UBSAN/TSAN is set.
if(TARGET ${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME} prev_sanitizers)
endif()

if(TARGET CompileSandboxShaders AND TARGET ${PROJECT_NAME})
    add_dependencies(${PROJECT_NAME} CompileSandboxShaders)
endif()
