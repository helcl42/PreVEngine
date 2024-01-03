cmake_minimum_required(VERSION 3.10)

project(PreVEngineTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_LIST 
    "*.h" "*.cpp"
    "common/*.h" "common/*.cpp"
    "common/intersection/*.h" "common/intersection/*.cpp"
    "component/*.h" "component/*.cpp"
    "component/transform/*.h" "component/transform/*.cpp"
    "component/common/*.h" "component/common/*.cpp"
    "component/time/*.h" "component/time/*.cpp"
    "component/terrain/*.h" "component/terrain/*.cpp"
    "component/sky/*.h" "component/sky/*.cpp"
    "component/cloud/*.h" "component/cloud/*.cpp"
    "component/render/*.h" "component/render/*.cpp"
    "component/light/*.h" "component/light/*.cpp"
    "component/font/*.h" "component/font/*.cpp"
    "component/camera/*.h" "component/camera/*.cpp"
    "component/shadow/*.h" "component/shadow/*.cpp"
    "component/particle/*.h" "component/particle/*.cpp"
    "component/water/*.h" "component/water/*.cpp"
    "component/ray_casting/*.h" "component/ray_casting/*.cpp"
    "render/*.h" "render/*.cpp"
    "render/pipeline/*.h" "render/pipeline/*.cpp"
    "render/util/*.h" "render/util/*.cpp"
    "render/util/assimp/*.h" "render/util/assimp/*.cpp"
    "render/mesh/*.h" "render/mesh/*.cpp"
    "render/model/*.h" "render/model/*.cpp"
    "render/material/*.h" "render/material/*.cpp"
    "render/animation/*.h" "render/animation/*.cpp"
    "render/font/*.h" "render/font/*.cpp"
    "render/renderer/*.h" "render/renderer/*.cpp"
    "render/renderer/animation/*.h" "render/renderer/animation/*.cpp"
    "render/renderer/animation/pipeline/*.h" "render/renderer/animation/pipeline/*.cpp"
    "render/renderer/animation/shader/*.h" "render/renderer/animation/shader/*.cpp"
    "render/renderer/debug/*.h" "render/renderer/debug/*.cpp"
    "render/renderer/debug/pipeline/*.h" "render/renderer/debug/pipeline/*.cpp"
    "render/renderer/debug/shader/*.h" "render/renderer/debug/shader/*.cpp"
    "render/renderer/common/*.h" "render/renderer/common/*.cpp"
    "render/renderer/common/pipeline/*.h" "render/renderer/common/pipeline/*.cpp"
    "render/renderer/common/shader/*.h" "render/renderer/common/shader/*.cpp"
    "render/renderer/normal/*.h" "render/renderer/normal/*.cpp"
    "render/renderer/normal/pipeline/*.h" "render/renderer/normal/pipeline/*.cpp"
    "render/renderer/normal/shader/*.h" "render/renderer/normal/shader/*.cpp"
    "render/renderer/font/*.h" "render/renderer/font/*.cpp"
    "render/renderer/font/pipeline/*.h" "render/renderer/font/pipeline/*.cpp"
    "render/renderer/font/shader/*.h" "render/renderer/font/shader/*.cpp"
    "render/renderer/particle/*.h" "render/renderer/particle/*.cpp"
    "render/renderer/particle/pipeline/*.h" "render/renderer/particle/pipeline/*.cpp"
    "render/renderer/particle/shader/*.h" "render/renderer/particle/shader/*.cpp"
    "render/renderer/shadow/*.h" "render/renderer/shadow/*.cpp"
    "render/renderer/shadow/pipeline/*.h" "render/renderer/shadow/pipeline/*.cpp"
    "render/renderer/shadow/shader/*.h" "render/renderer/shadow/shader/*.cpp"
    "render/renderer/sky/*.h" "render/renderer/sky/*.cpp"
    "render/renderer/sky/pipeline/*.h" "render/renderer/sky/pipeline/*.cpp"
    "render/renderer/sky/shader/*.h" "render/renderer/sky/shader/*.cpp"
    "render/renderer/terrain/*.h" "render/renderer/terrain/*.cpp"
    "render/renderer/terrain/pipeline/*.h" "render/renderer/terrain/pipeline/*.cpp"
    "render/renderer/terrain/shader/*.h" "render/renderer/terrain/shader/*.cpp"
    "render/renderer/water/*.h" "render/renderer/water/*.cpp"
    "render/renderer/water/pipeline/*.h" "render/renderer/water/pipeline/*.cpp"
    "render/renderer/water/shader/*.h" "render/renderer/water/shader/*.cpp"
    "scene/*.h" "scene/*.cpp"
    "scene/light/*.h" "scene/light/*.cpp"
    "scene/ray_casting/*.h" "scene/ray_casting/*.cpp"
    "scene/robot/*.h" "scene/robot/*.cpp"
    "scene/shadow/*.h" "scene/shadow/*.cpp"
    "scene/sky/*.h" "scene/sky/*.cpp"
    "scene/terrain/*.h" "scene/terrain/*.cpp"
    "scene/water/*.h" "scene/water/*.cpp"
)

# Options to set
option(RENDER_SELECTION "Render selection" ON)
option(RENDER_RAYCASTS "Render ray casts" OFF)
option(RENDER_BOUNDING_VOLUMES "Render bounding volumes" OFF)
option(PARALLEL_RENDERING "Parallel rendering" ON)

if (RENDER_SELECTION)
    add_definitions(-DRENDER_SELECTION)
endif()
if (RENDER_RAYCASTS)
    add_definitions(-DRENDER_RAYCASTS)
endif()
if (RENDER_BOUNDING_VOLUMES)
    add_definitions(-DRENDER_BOUNDING_VOLUMES)
endif()
if (PARALLEL_RENDERING)
    add_definitions(-DPARALLEL_RENDERING)
endif()
# Options derived from PreVEngine
if (ENABLE_REVERSE_DEPTH)
    add_definitions(-DENABLE_REVERSE_DEPTH)
endif()

add_definitions(-DVK_NO_PROTOTYPES)

include_directories("../PreVEngine/external/worm_holes/WormHoles")
include_directories("../PreVEngine/external/glm")
include_directories("../Dependencies/assimp/include")

if(NOT TARGET PreVEngine)
    add_subdirectory("../PreVEngine" "${CMAKE_BINARY_DIR}/PreVEngine")
endif()

if(WIN32)
    add_executable(${PROJECT_NAME} ${SRC_LIST})

    target_link_libraries(${PROJECT_NAME} PreVEngine)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Windows/assimp-vc142-mt.lib")

    # Copy assimp dll to output bin dir.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Windows/assimp-vc142-mt.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    # Copy all files from ./Assets to the binary directory.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    add_executable(${PROJECT_NAME} ${SRC_LIST})

    target_link_libraries(${PROJECT_NAME} PreVEngine)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Linux/libassimp.so")

    # Copy assimp so to output bin dir.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Linux/libassimp.so" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    #  Copy all files from ./Assets to the binary directory.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif()

if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O3")

    add_definitions(-DGLM_FORCE_CXX17)

    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../PreVEngine/platform/android")
    include_directories("${ANDROID_NDK}/sources/android/native_app_glue")

    add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

    target_link_libraries(${PROJECT_NAME} PreVEngine)

    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Android/libassimp.so" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/platform/android/app/src/main/libs/arm64-v8a/")

    # add external library
    add_library(assimp SHARED IMPORTED)
    set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/platform/android/app/src/main/libs/arm64-v8a/libassimp.so")

    # Link to external libraries
    target_link_libraries(${PROJECT_NAME} log android assimp)
endif()

# testing
enable_testing()

add_test(NAME DefaultTest COMMAND $<TARGET_FILE:${PROJECT_NAME}>)

