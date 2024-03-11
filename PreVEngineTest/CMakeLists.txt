cmake_minimum_required(VERSION 3.10)

project(PreVEngineTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_LIST 
    "prev_test/*.h" "prev_test/*.cpp"
    "prev_test/common/*.h" "prev_test/common/*.cpp"
    "prev_test/common/intersection/*.h" "prev_test/common/intersection/*.cpp"
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
    "prev_test/render/*.h" "prev_test/render/*.cpp"
    "prev_test/render/pipeline/*.h" "prev_test/render/pipeline/*.cpp"
    "prev_test/render/util/*.h" "prev_test/render/util/*.cpp"
    "prev_test/render/util/assimp/*.h" "prev_test/render/util/assimp/*.cpp"
    "prev_test/render/mesh/*.h" "prev_test/render/mesh/*.cpp"
    "prev_test/render/model/*.h" "prev_test/render/model/*.cpp"
    "prev_test/render/material/*.h" "prev_test/render/material/*.cpp"
    "prev_test/render/animation/*.h" "prev_test/render/animation/*.cpp"
    "prev_test/render/font/*.h" "prev_test/render/font/*.cpp"
    "prev_test/render/renderer/*.h" "prev_test/render/renderer/*.cpp"
    "prev_test/render/renderer/animation/*.h" "prev_test/render/renderer/animation/*.cpp"
    "prev_test/render/renderer/animation/pipeline/*.h" "prev_test/render/renderer/animation/pipeline/*.cpp"
    "prev_test/render/renderer/animation/shader/*.h" "prev_test/render/renderer/animation/shader/*.cpp"
    "prev_test/render/renderer/debug/*.h" "prev_test/render/renderer/debug/*.cpp"
    "prev_test/render/renderer/debug/pipeline/*.h" "prev_test/render/renderer/debug/pipeline/*.cpp"
    "prev_test/render/renderer/debug/shader/*.h" "prev_test/render/renderer/debug/shader/*.cpp"
    "prev_test/render/renderer/common/*.h" "prev_test/render/renderer/common/*.cpp"
    "prev_test/render/renderer/common/pipeline/*.h" "prev_test/render/renderer/common/pipeline/*.cpp"
    "prev_test/render/renderer/common/shader/*.h" "prev_test/render/renderer/common/shader/*.cpp"
    "prev_test/render/renderer/normal/*.h" "prev_test/render/renderer/normal/*.cpp"
    "prev_test/render/renderer/normal/pipeline/*.h" "prev_test/render/renderer/normal/pipeline/*.cpp"
    "prev_test/render/renderer/normal/shader/*.h" "prev_test/render/renderer/normal/shader/*.cpp"
    "prev_test/render/renderer/font/*.h" "prev_test/render/renderer/font/*.cpp"
    "prev_test/render/renderer/font/pipeline/*.h" "prev_test/render/renderer/font/pipeline/*.cpp"
    "prev_test/render/renderer/font/shader/*.h" "prev_test/render/renderer/font/shader/*.cpp"
    "prev_test/render/renderer/particle/*.h" "prev_test/render/renderer/particle/*.cpp"
    "prev_test/render/renderer/particle/pipeline/*.h" "prev_test/render/renderer/particle/pipeline/*.cpp"
    "prev_test/render/renderer/particle/shader/*.h" "prev_test/render/renderer/particle/shader/*.cpp"
    "prev_test/render/renderer/shadow/*.h" "prev_test/render/renderer/shadow/*.cpp"
    "prev_test/render/renderer/shadow/pipeline/*.h" "prev_test/render/renderer/shadow/pipeline/*.cpp"
    "prev_test/render/renderer/shadow/shader/*.h" "prev_test/render/renderer/shadow/shader/*.cpp"
    "prev_test/render/renderer/sky/*.h" "prev_test/render/renderer/sky/*.cpp"
    "prev_test/render/renderer/sky/pipeline/*.h" "prev_test/render/renderer/sky/pipeline/*.cpp"
    "prev_test/render/renderer/sky/shader/*.h" "prev_test/render/renderer/sky/shader/*.cpp"
    "prev_test/render/renderer/terrain/*.h" "prev_test/render/renderer/terrain/*.cpp"
    "prev_test/render/renderer/terrain/pipeline/*.h" "prev_test/render/renderer/terrain/pipeline/*.cpp"
    "prev_test/render/renderer/terrain/shader/*.h" "prev_test/render/renderer/terrain/shader/*.cpp"
    "prev_test/render/renderer/water/*.h" "prev_test/render/renderer/water/*.cpp"
    "prev_test/render/renderer/water/pipeline/*.h" "prev_test/render/renderer/water/pipeline/*.cpp"
    "prev_test/render/renderer/water/shader/*.h" "prev_test/render/renderer/water/shader/*.cpp"
    "prev_test/scene/*.h" "prev_test/scene/*.cpp"
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
if (ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
endif()

add_definitions(-DVK_NO_PROTOTYPES)

include_directories("../PreVEngine")
include_directories("../PreVEngine/external/worm_holes/WormHoles")
include_directories("../PreVEngine/external/glm")
include_directories("../Dependencies/assimp/include")

if(WIN32)
    add_executable(${PROJECT_NAME} ${SRC_LIST})

    target_link_libraries(${PROJECT_NAME} PreVEngine)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Windows/assimp-vc142-mt.lib")

    # Copy assimp dll to output bin dir.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Windows/assimp-vc142-mt.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    # Copy all files from ./Assets to the binary directory.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    add_executable(${PROJECT_NAME} ${SRC_LIST})

    target_link_libraries(${PROJECT_NAME} PreVEngine)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Linux/libassimp.so")

    # Copy assimp so to output bin dir.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Linux/libassimp.so" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    #  Copy all files from ./Assets to the binary directory.
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

elseif(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O3")

    add_definitions(-DGLM_FORCE_CXX17)

    add_subdirectory("${CMAKE_BINARY_DIR}/PreVEngine")

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

