cmake_minimum_required(VERSION 3.10)

project(PreVEngineExample)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_LIST 
    "prev_test/*.h" "prev_test/*.cpp"
    "prev_test/common/*.h" "prev_test/common/*.cpp"
    "prev_test/component/*.h" "prev_test/component/*.cpp"
    "prev_test/component/transform/*.h" "prev_test/component/transform/*.cpp"
    "prev_test/component/common/*.h" "prev_test/component/common/*.cpp"
    "prev_test/component/time/*.h" "prev_test/component/time/*.cpp"
    "prev_test/component/terrain/*.h" "prev_test/component/terrain/*.cpp"
    "prev_test/component/sky/*.h" "prev_test/component/sky/*.cpp"
    "prev_test/component/sky/cloud/*.h" "prev_test/component/sky/cloud/*.cpp"
    "prev_test/component/render/*.h" "prev_test/component/render/*.cpp"
    "prev_test/component/light/*.h" "prev_test/component/light/*.cpp"
    "prev_test/component/font/*.h" "prev_test/component/font/*.cpp"
    "prev_test/component/camera/*.h" "prev_test/component/camera/*.cpp"
    "prev_test/component/shadow/*.h" "prev_test/component/shadow/*.cpp"
    "prev_test/component/particle/*.h" "prev_test/component/particle/*.cpp"
    "prev_test/component/water/*.h" "prev_test/component/water/*.cpp"
    "prev_test/component/ray_casting/*.h" "prev_test/component/ray_casting/*.cpp"
    "prev_test/component/hand_tracking/*.h" "prev_test/component/hand_tracking/*.cpp"
    "prev_test/render/*.h" "prev_test/render/*.cpp"
    "prev_test/render/util/*.h" "prev_test/render/util/*.cpp"
    "prev_test/render/util/assimp/*.h" "prev_test/render/util/assimp/*.cpp"
    "prev_test/render/mesh/*.h" "prev_test/render/mesh/*.cpp"
    "prev_test/render/model/*.h" "prev_test/render/model/*.cpp"
    "prev_test/render/material/*.h" "prev_test/render/material/*.cpp"
    "prev_test/render/animation/*.h" "prev_test/render/animation/*.cpp"
    "prev_test/render/font/*.h" "prev_test/render/font/*.cpp"
    "prev_test/render/renderer/*.h" "prev_test/render/renderer/*.cpp"
    "prev_test/render/renderer/animation/*.h" "prev_test/render/renderer/animation/*.cpp"
    "prev_test/render/renderer/debug/*.h" "prev_test/render/renderer/debug/*.cpp"
    "prev_test/render/renderer/normal/*.h" "prev_test/render/renderer/normal/*.cpp"
    "prev_test/render/renderer/font/*.h" "prev_test/render/renderer/font/*.cpp"
    "prev_test/render/renderer/particle/*.h" "prev_test/render/renderer/particle/*.cpp"
    "prev_test/render/renderer/shadow/*.h" "prev_test/render/renderer/shadow/*.cpp"
    "prev_test/render/renderer/sky/*.h" "prev_test/render/renderer/sky/*.cpp"
    "prev_test/render/renderer/terrain/*.h" "prev_test/render/renderer/terrain/*.cpp"
    "prev_test/render/renderer/water/*.h" "prev_test/render/renderer/water/*.cpp"
    "prev_test/render/renderer/xr/*.h" "prev_test/render/renderer/xr/*.cpp"
    "prev_test/scene/*.h" "prev_test/scene/*.cpp"
    "prev_test/scene/text/*.h" "prev_test/scene/text/*.cpp"
    "prev_test/scene/light/*.h" "prev_test/scene/light/*.cpp"
    "prev_test/scene/ray_casting/*.h" "prev_test/scene/ray_casting/*.cpp"
    "prev_test/scene/robot/*.h" "prev_test/scene/robot/*.cpp"
    "prev_test/scene/shadow/*.h" "prev_test/scene/shadow/*.cpp"
    "prev_test/scene/sky/*.h" "prev_test/scene/sky/*.cpp"
    "prev_test/scene/terrain/*.h" "prev_test/scene/terrain/*.cpp"
    "prev_test/scene/water/*.h" "prev_test/scene/water/*.cpp"
)

# Options to set
option(RENDER_SELECTION "Render selection" ON)
option(RENDER_RAYCASTS "Render ray casts" OFF)
option(RENDER_BOUNDING_VOLUMES "Render bounding volumes" OFF)
option(PARALLEL_COMMAND_RECORDING "Parallel rendering" OFF)

if (RENDER_SELECTION)
    add_definitions(-DRENDER_SELECTION)
endif()
if (RENDER_RAYCASTS)
    add_definitions(-DRENDER_RAYCASTS)
endif()
if (RENDER_BOUNDING_VOLUMES)
    add_definitions(-DRENDER_BOUNDING_VOLUMES)
endif()
if (PARALLEL_COMMAND_RECORDING)
    add_definitions(-DPARALLEL_COMMAND_RECORDING)
endif()
# Options derived from PreVEngine
if (ENABLE_REVERSE_DEPTH)
    add_definitions(-DENABLE_REVERSE_DEPTH)
endif()
if (ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
endif()
if(ENABLE_XR)
    add_definitions(-DENABLE_XR)
endif()

add_subdirectory(assets/Shaders)

include_directories("../../PreVEngine")
include_directories("../../PreVEngine/external")
include_directories("../../PreVEngine/external/worm_holes/WormHoles")
include_directories("../../PreVEngine/external/gfx/gfx/include")
include_directories("../../PreVEngine/external/glm")

# Build assimp from source (v6.0.5) for all platforms
include(FetchContent)
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v6.0.5
    GIT_SHALLOW    TRUE
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
if(EMSCRIPTEN OR WIN32)
    set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
else()
    set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
endif()
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

if(WIN32)
    message("${PROJECT_NAME} - Windows")
    add_definitions(-DTARGET_PLATFORM_WINDOWS)
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    file(GLOB COMMON_SRC_LIST 
        "platform/common/*.h" "platform/common/*.cpp"
    )

    add_executable(${PROJECT_NAME} 
        ${SRC_LIST}
        ${COMMON_SRC_LIST}
    )

    target_link_libraries(${PROJECT_NAME} PreVEngine assimp)
    target_include_directories(${PROJECT_NAME} PRIVATE ${assimp_SOURCE_DIR}/include ${assimp_BINARY_DIR}/include)

    # Copy all files from ./Assets to the binary directory.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

elseif(IOS)
    message("${PROJECT_NAME} - iOS")
    add_definitions(-DTARGET_PLATFORM_IOS)

    set(APP_BUNDLE_IDENTIFIER "com.prevengine.PreVEngineExample")
    set(MACOSX_BUNDLE_INFO_STRING ${APP_BUNDLE_IDENTIFIER})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")

    set(IOS_LAUNCH_STORYBOARD "${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/Launch Screen.storyboard")

    find_library(UIKIT_LIB UIKit)
    find_library(METALKIT_LIB METALKit)
    find_library(FOUNDATION_LIB Foundation)

    file(GLOB IOS_SRC_LIST 
        "platform/ios/*.h" "platform/ios/*.mm"
    )

    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../PreVEngine/platform/ios")

    add_executable(${PROJECT_NAME} 
        ${SRC_LIST}
        ${IOS_SRC_LIST}
        ${IOS_LAUNCH_STORYBOARD}
    )

    target_link_libraries(${PROJECT_NAME} PreVEngine assimp)
    target_include_directories(${PROJECT_NAME} PRIVATE ${assimp_SOURCE_DIR}/include ${assimp_BINARY_DIR}/include)
    if(${CMAKE_OSX_SYSROOT} MATCHES "iPhoneSimulator")
        message("${PROJECT_NAME} - simulator")
        if (${CMAKE_OSX_ARCHITECTURES} MATCHES "arm64")
            target_link_libraries(${PROJECT_NAME} "-framework $ENV{VULKAN_SDK}/../iOS/lib/MoltenVK.xcframework/ios-arm64_x86_64-simulator/MoltenVK.framework/MoltenVK")
        else()
            message(FATAL_ERROR "Unsupported architecture: ${CMAKE_OSX_ARCHITECTURES}.")
        endif()    
    else()
        message("${PROJECT_NAME} - iphone")
        if (${CMAKE_OSX_ARCHITECTURES} MATCHES "arm64")
            # target_link_libraries(${PROJECT_NAME} "-framework $ENV{VULKAN_SDK}/../iOS/lib/MoltenVK.xcframework/ios-arm64/MoltenVK.framework/MoltenVK")
        else()
            message(FATAL_ERROR "Unsupported architecture: ${CMAKE_OSX_ARCHITECTURES}.")
        endif()
    endif()

    target_link_libraries(${PROJECT_NAME} 
        ${UIKIT_LIB}
        ${METALKIT_LIB}
        ${FOUNDATION_LIB}
    )

    # Add assets as bundle resources for iOS (preserving directory structure)
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
    message("${PROJECT_NAME} - MacOS")
    add_definitions(-DTARGET_PLATFORM_MACOS)

    file(GLOB COMMON_SRC_LIST 
        "platform/common/*.h" "platform/common/*.cpp"
    )

    add_executable(${PROJECT_NAME} 
        ${SRC_LIST}
        ${COMMON_SRC_LIST}
    )

    target_link_libraries(${PROJECT_NAME} PreVEngine assimp)
    target_include_directories(${PROJECT_NAME} PRIVATE ${assimp_SOURCE_DIR}/include ${assimp_BINARY_DIR}/include)

    #  Copy all files from ./Assets to the binary directory.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    message("${PROJECT_NAME} - Linux")
    add_definitions(-DTARGET_PLATFORM_LINUX)

    file(GLOB COMMON_SRC_LIST 
        "platform/common/*.h" "platform/common/*.cpp"
    )

    add_executable(${PROJECT_NAME} 
        ${SRC_LIST}
        ${COMMON_SRC_LIST}
    )

    target_link_libraries(${PROJECT_NAME} PreVEngine assimp)
    target_include_directories(${PROJECT_NAME} PRIVATE ${assimp_SOURCE_DIR}/include ${assimp_BINARY_DIR}/include)

    #  Copy all files from ./Assets to the binary directory.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

elseif(ANDROID)
    message("${PROJECT_NAME} - Android")
    add_definitions(-DTARGET_PLATFORM_ANDROID)

    file(GLOB COMMON_SRC_LIST 
        "platform/android/app/src/main/cpp/*.h" "platform/android/app/src/main/cpp/*.cpp"
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O3")

    add_definitions(-DGLM_FORCE_CXX17)

    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../PreVEngine/platform/android")
    include_directories("${ANDROID_NDK}/sources/android/native_app_glue")

    add_library(${PROJECT_NAME} SHARED 
        ${SRC_LIST}
        ${COMMON_SRC_LIST}
    )

    target_link_libraries(${PROJECT_NAME} PreVEngine log android assimp)
    target_include_directories(${PROJECT_NAME} PRIVATE ${assimp_SOURCE_DIR}/include ${assimp_BINARY_DIR}/include)

elseif(EMSCRIPTEN)
    message("${PROJECT_NAME} - Emscripten/Web")
    add_definitions(-DTARGET_PLATFORM_EMSCRIPTEN)
    add_definitions(-DWORM_HOLES_SINGLE_THREADED)

    file(GLOB COMMON_SRC_LIST 
        "platform/common/*.h" "platform/common/*.cpp"
    )

    add_executable(${PROJECT_NAME} 
        ${SRC_LIST}
        ${COMMON_SRC_LIST}
    )

    target_link_libraries(${PROJECT_NAME} PreVEngine assimp)
    target_include_directories(${PROJECT_NAME} PRIVATE ${assimp_SOURCE_DIR}/include ${assimp_BINARY_DIR}/include)

    # Emscripten link flags
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

else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

# Sanitizers (see top-level CMakeLists). No-op unless ENABLE_ASAN/UBSAN/TSAN is set.
target_link_libraries(${PROJECT_NAME} prev_sanitizers)

if(TARGET CompileShaders AND TARGET ${PROJECT_NAME})
    add_dependencies(${PROJECT_NAME} CompileShaders)
endif()

if(ENABLE_XR)
    message("${PROJECT_NAME} - Using XR")
else()
    message("${PROJECT_NAME} - Not using XR")
endif()

