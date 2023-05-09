cmake_minimum_required(VERSION 3.0)

project(PreVEngineTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

include_directories("../Dependencies/glm")
include_directories("../Dependencies/assimp/include")

file(GLOB ROOT_SRC_LIST "*.h" "*.cpp")

file(GLOB COMMON_SRC_LIST "common/*.h" "common/*.cpp")
file(GLOB COMMON_INTERSECTION_SRC_LIST "common/intersection/*.h" "common/intersection/*.cpp")

file(GLOB COMPONENT_SRC_LIST "component/*.h" "component/*.cpp")
file(GLOB COMPONENT_TRANSFORM_SRC_LIST "component/transform/*.h" "component/transform/*.cpp")
file(GLOB COMPONENT_COMMON_SRC_LIST "component/common/*.h" "component/common/*.cpp")
file(GLOB COMPONENT_TIME_SRC_LIST "component/time/*.h" "component/time/*.cpp")
file(GLOB COMPONENT_TERRAIN_SRC_LIST "component/terrain/*.h" "component/terrain/*.cpp")
file(GLOB COMPONENT_SKY_SRC_LIST "component/sky/*.h" "component/sky/*.cpp")
file(GLOB COMPONENT_CLOUD_SRC_LIST "component/cloud/*.h" "component/cloud/*.cpp")
file(GLOB COMPONENT_RENDER_SRC_LIST "component/render/*.h" "component/render/*.cpp")
file(GLOB COMPONENT_LIGHT_SRC_LIST "component/light/*.h" "component/light/*.cpp")
file(GLOB COMPONENT_FONT_SRC_LIST "component/font/*.h" "component/font/*.cpp")
file(GLOB COMPONENT_CAMERA_SRC_LIST "component/camera/*.h" "component/camera/*.cpp")
file(GLOB COMPONENT_SHADOW_SRC_LIST "component/shadow/*.h" "component/shadow/*.cpp")
file(GLOB COMPONENT_PARTICLE_SRC_LIST "component/particle/*.h" "component/particle/*.cpp")
file(GLOB COMPONENT_WATER_SRC_LIST "component/water/*.h" "component/water/*.cpp")
file(GLOB COMPONENT_RAY_CASTING_SRC_LIST "component/ray_casting/*.h" "component/ray_casting/*.cpp")

file(GLOB RENDER_SRC_LIST "render/*.h" "render/*.cpp")
file(GLOB RENDER_PIPELINE_SRC_LIST "render/pipeline/*.h" "render/pipeline/*.cpp")

file(GLOB RENDER_UTIL_SRC_LIST "render/util/*.h" "render/util/*.cpp")
file(GLOB RENDER_UTIL_ASSIMP_SRC_LIST "render/util/assimp/*.h" "render/util/assimp/*.cpp")

file(GLOB RENDER_MESH_SRC_LIST "render/mesh/*.h" "render/mesh/*.cpp")
file(GLOB RENDER_MODEL_SRC_LIST "render/model/*.h" "render/model/*.cpp")
file(GLOB RENDER_MATERIAL_SRC_LIST "render/material/*.h" "render/material/*.cpp")
file(GLOB RENDER_ANIMATION_SRC_LIST "render/animation/*.h" "render/animation/*.cpp")
file(GLOB RENDER_FONT_SRC_LIST "render/font/*.h" "render/font/*.cpp")

file(GLOB RENDER_RENDERER_SRC_LIST "render/renderer/*.h" "render/renderer/*.cpp")

file(GLOB RENDER_RENDERER_ANIMATION_SRC_LIST "render/renderer/animation/*.h" "render/renderer/animation/*.cpp")
file(GLOB RENDER_RENDERER_ANIMATION_PIPELINE_SRC_LIST "render/renderer/animation/pipeline/*.h" "render/renderer/animation/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_ANIMATION_SHADER_SRC_LIST "render/renderer/animation/shader/*.h" "render/renderer/animation/shader/*.cpp")

file(GLOB RENDER_RENDERER_DEBUG_SRC_LIST "render/renderer/debug/*.h" "render/renderer/debug/*.cpp")
file(GLOB RENDER_RENDERER_DEBUG_PIPELINE_SRC_LIST "render/renderer/debug/pipeline/*.h" "render/renderer/debug/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_DEBUG_SHADER_SRC_LIST "render/renderer/debug/shader/*.h" "render/renderer/debug/shader/*.cpp")

file(GLOB RENDER_RENDERER_COMMON_SRC_LIST "render/renderer/common/*.h" "render/renderer/common/*.cpp")
file(GLOB RENDER_RENDERER_COMMON_PIPELINE_SRC_LIST "render/renderer/common/pipeline/*.h" "render/renderer/common/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_COMMON_SHADER_SRC_LIST "render/renderer/common/shader/*.h" "render/renderer/common/shader/*.cpp")

file(GLOB RENDER_RENDERER_NORMAL_SRC_LIST "render/renderer/normal/*.h" "render/renderer/normal/*.cpp")
file(GLOB RENDER_RENDERER_NORMAL_PIPELINE_SRC_LIST "render/renderer/normal/pipeline/*.h" "render/renderer/normal/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_NORMAL_SHADER_SRC_LIST "render/renderer/normal/shader/*.h" "render/renderer/normal/shader/*.cpp")

file(GLOB RENDER_RENDERER_FONT_SRC_LIST "render/renderer/font/*.h" "render/renderer/font/*.cpp")
file(GLOB RENDER_RENDERER_FONT_PIPELINE_SRC_LIST "render/renderer/font/pipeline/*.h" "render/renderer/font/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_FONT_SHADER_SRC_LIST "render/renderer/font/shader/*.h" "render/renderer/font/shader/*.cpp")

file(GLOB RENDER_RENDERER_PARTICLE_SRC_LIST "render/renderer/particle/*.h" "render/renderer/particle/*.cpp")
file(GLOB RENDER_RENDERER_PARTICLE_PIPELINE_SRC_LIST "render/renderer/particle/pipeline/*.h" "render/renderer/particle/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_PARTICLE_SHADER_SRC_LIST "render/renderer/particle/shader/*.h" "render/renderer/particle/shader/*.cpp")

file(GLOB RENDER_RENDERER_SHADOW_SRC_LIST "render/renderer/shadow/*.h" "render/renderer/shadow/*.cpp")
file(GLOB RENDER_RENDERER_SHADOW_PIPELINE_SRC_LIST "render/renderer/shadow/pipeline/*.h" "render/renderer/shadow/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_SHADOW_SHADER_SRC_LIST "render/renderer/shadow/shader/*.h" "render/renderer/shadow/shader/*.cpp")

file(GLOB RENDER_RENDERER_SKY_SRC_LIST "render/renderer/sky/*.h" "render/renderer/sky/*.cpp")
file(GLOB RENDER_RENDERER_SKY_PIPELINE_SRC_LIST "render/renderer/sky/pipeline/*.h" "render/renderer/sky/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_SKY_SHADER_SRC_LIST "render/renderer/sky/shader/*.h" "render/renderer/sky/shader/*.cpp")

file(GLOB RENDER_RENDERER_TERRAIN_SRC_LIST "render/renderer/terrain/*.h" "render/renderer/terrain/*.cpp")
file(GLOB RENDER_RENDERER_TERRAIN_PIPELINE_SRC_LIST "render/renderer/terrain/pipeline/*.h" "render/renderer/terrain/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_TERRAIN_SHADER_SRC_LIST "render/renderer/terrain/shader/*.h" "render/renderer/terrain/shader/*.cpp")

file(GLOB RENDER_RENDERER_WATER_SRC_LIST "render/renderer/water/*.h" "render/renderer/water/*.cpp")
file(GLOB RENDER_RENDERER_WATER_PIPELINE_SRC_LIST "render/renderer/water/pipeline/*.h" "render/renderer/water/pipeline/*.cpp")
file(GLOB RENDER_RENDERER_WATER_SHADER_SRC_LIST "render/renderer/water/shader/*.h" "render/renderer/water/shader/*.cpp")

file(GLOB SCENE_SRC_LIST "scene/*.h" "scene/*.cpp")
file(GLOB SCENE_LIGHT_SRC_LIST "scene/light/*.h" "scene/light/*.cpp")
file(GLOB SCENE_RAY_CASTING_SRC_LIST "scene/ray_casting/*.h" "scene/ray_casting/*.cpp")
file(GLOB SCENE_ROBOT_SRC_LIST "scene/robot/*.h" "scene/robot/*.cpp")
file(GLOB SCENE_SHADOW_SRC_LIST "scene/shadow/*.h" "scene/shadow/*.cpp")
file(GLOB SCENE_SKY_SRC_LIST "scene/sky/*.h" "scene/sky/*.cpp")
file(GLOB SCENE_TERRAIN_SRC_LIST "scene/terrain/*.h" "scene/terrain/*.cpp")
file(GLOB SCENE_WATER_SRC_LIST "scene/water/*.h" "scene/water/*.cpp")

source_group("" FILES ${ROOT_SRC_LIST})

source_group("common" FILES ${COMMON_SRC_LIST})
source_group("common/intersection" FILES ${COMMON_INTERSECTION_SRC_LIST})

source_group("component" FILES ${COMPONENT_SRC_LIST})
source_group("component/transform" FILES ${COMPONENT_TRANSFORM_SRC_LIST})
source_group("component/common" FILES ${COMPONENT_COMMON_SRC_LIST})
source_group("component/time" FILES ${COMPONENT_TIME_SRC_LIST})
source_group("component/terrain" FILES ${COMPONENT_TERRAIN_SRC_LIST})
source_group("component/sky" FILES ${COMPONENT_SKY_SRC_LIST})
source_group("component/cloud" FILES ${COMPONENT_CLOUD_SRC_LIST})
source_group("component/render" FILES ${COMPONENT_RENDER_SRC_LIST})
source_group("component/light" FILES ${COMPONENT_LIGHT_SRC_LIST})
source_group("component/font" FILES ${COMPONENT_FONT_SRC_LIST})
source_group("component/camera" FILES ${COMPONENT_CAMERA_SRC_LIST})
source_group("component/shadow" FILES ${COMPONENT_SHADOW_SRC_LIST})
source_group("component/particle" FILES ${COMPONENT_PARTICLE_SRC_LIST})
source_group("component/water" FILES ${COMPONENT_WATER_SRC_LIST})
source_group("component/ray_casting" FILES ${COMPONENT_RAY_CASTING_SRC_LIST})

source_group("render" FILES ${RENDER_SRC_LIST})
source_group("render/pipeline" FILES ${RENDER_PIPELINE_SRC_LIST})

source_group("render/util" FILES ${RENDER_UTIL_SRC_LIST})
source_group("render/util/assimp" FILES ${RENDER_UTIL_ASSIMP_SRC_LIST})

source_group("render/mesh" FILES ${RENDER_MESH_SRC_LIST})
source_group("render/model" FILES ${RENDER_MODEL_SRC_LIST})
source_group("render/material" FILES ${RENDER_MATERIAL_SRC_LIST})
source_group("render/animation" FILES ${RENDER_ANIMATION_SRC_LIST})
source_group("render/font" FILES ${RENDER_FONT_SRC_LIST})

source_group("render/renderer" FILES ${RENDER_RENDERER_SRC_LIST})

source_group("render/renderer/animation" FILES ${RENDER_RENDERER_ANIMATION_SRC_LIST})
source_group("render/renderer/animation/pipeline" FILES ${RENDER_RENDERER_ANIMATION_PIPELINE_SRC_LIST})
source_group("render/renderer/animation/shader" FILES ${RENDER_RENDERER_ANIMATION_SHADER_SRC_LIST})

source_group("render/renderer/debug" FILES ${RENDER_RENDERER_DEBUG_SRC_LIST})
source_group("render/renderer/debug/pipeline" FILES ${RENDER_RENDERER_DEBUG_PIPELINE_SRC_LIST})
source_group("render/renderer/debug/shader" FILES ${RENDER_RENDERER_DEBUG_SHADER_SRC_LIST})

source_group("render/renderer/common" FILES ${RENDER_RENDERER_COMMON_SRC_LIST})
source_group("render/renderer/common/pipeline" FILES ${RENDER_RENDERER_COMMON_PIPELINE_SRC_LIST})
source_group("render/renderer/common/shader" FILES ${RENDER_RENDERER_COMMON_SHADER_SRC_LIST})

source_group("render/renderer/normal" FILES ${RENDER_RENDERER_NORMAL_SRC_LIST})
source_group("render/renderer/normal/pipeline" FILES ${RENDER_RENDERER_NORMAL_PIPELINE_SRC_LIST})
source_group("render/renderer/normal/shader" FILES ${RENDER_RENDERER_NORMAL_SHADER_SRC_LIST})

source_group("render/renderer/font" FILES ${RENDER_RENDERER_FONT_SRC_LIST})
source_group("render/renderer/font/pipeline" FILES ${RENDER_RENDERER_FONT_PIPELINE_SRC_LIST})
source_group("render/renderer/font/shader" FILES ${RENDER_RENDERER_FONT_SHADER_SRC_LIST})

source_group("render/renderer/particle" FILES ${RENDER_RENDERER_PARTICLE_SRC_LIST})
source_group("render/renderer/particle/pipeline" FILES ${RENDER_RENDERER_PARTICLE_PIPELINE_SRC_LIST})
source_group("render/renderer/particle/shader" FILES ${RENDER_RENDERER_PARTICLE_SHADER_SRC_LIST})

source_group("render/renderer/shadow" FILES ${RENDER_RENDERER_SHADOW_SRC_LIST})
source_group("render/renderer/shadow/pipeline" FILES ${RENDER_RENDERER_SHADOW_PIPELINE_SRC_LIST})
source_group("render/renderer/shadow/shader" FILES ${RENDER_RENDERER_SHADOW_SHADER_SRC_LIST})

source_group("render/renderer/sky" FILES ${RENDER_RENDERER_SKY_SRC_LIST})
source_group("render/renderer/sky/pipeline" FILES ${RENDER_RENDERER_SKY_PIPELINE_SRC_LIST})
source_group("render/renderer/sky/shader" FILES ${RENDER_RENDERER_SKY_SHADER_SRC_LIST})

source_group("render/renderer/terrain" FILES ${RENDER_RENDERER_TERRAIN_SRC_LIST})
source_group("render/renderer/terrain/pipeline" FILES ${RENDER_RENDERER_TERRAIN_PIPELINE_SRC_LIST})
source_group("render/renderer/terrain/shader" FILES ${RENDER_RENDERER_TERRAIN_SHADER_SRC_LIST})

source_group("render/renderer/water" FILES ${RENDER_RENDERER_WATER_SRC_LIST})
source_group("render/renderer/water/pipeline" FILES ${RENDER_RENDERER_WATER_PIPELINE_SRC_LIST})
source_group("render/renderer/water/shader" FILES ${RENDER_RENDERER_WATER_SHADER_SRC_LIST})

source_group("scene" FILES ${SCENE_SRC_LIST})
source_group("scene/light" FILES ${SCENE_LIGHT_SRC_LIST})
source_group("scene/ray_casting" FILES ${SCENE_RAY_CASTING_SRC_LIST})
source_group("scene/robot" FILES ${SCENE_ROBOT_SRC_LIST})
source_group("scene/shadow" FILES ${SCENE_SHADOW_SRC_LIST})
source_group("scene/sky" FILES ${SCENE_SKY_SRC_LIST})
source_group("scene/terrain" FILES ${SCENE_TERRAIN_SRC_LIST})
source_group("scene/water" FILES ${SCENE_WATER_SRC_LIST})

add_executable(${PROJECT_NAME} 
    ${ROOT_SRC_LIST}
    ${COMMON_SRC_LIST}
    ${COMMON_INTERSECTION_SRC_LIST}
    ${COMPONENT_SRC_LIST}
    ${COMPONENT_TRANSFORM_SRC_LIST}
    ${COMPONENT_COMMON_SRC_LIST}
    ${COMPONENT_TIME_SRC_LIST}
    ${COMPONENT_TERRAIN_SRC_LIST}
    ${COMPONENT_SKY_SRC_LIST}
    ${COMPONENT_CLOUD_SRC_LIST}
    ${COMPONENT_RENDER_SRC_LIST}
    ${COMPONENT_LIGHT_SRC_LIST}
    ${COMPONENT_FONT_SRC_LIST}
    ${COMPONENT_CAMERA_SRC_LIST}
    ${COMPONENT_SHADOW_SRC_LIST}
    ${COMPONENT_PARTICLE_SRC_LIST}
    ${COMPONENT_WATER_SRC_LIST}
    ${COMPONENT_RAY_CASTING_SRC_LIST}
    ${RENDER_SRC_LIST}
    ${RENDER_UTIL_SRC_LIST}
    ${RENDER_UTIL_ASSIMP_SRC_LIST}
    ${RENDER_MESH_SRC_LIST}
    ${RENDER_MODEL_SRC_LIST}
    ${RENDER_MATERIAL_SRC_LIST}
    ${RENDER_ANIMATION_SRC_LIST}
    ${RENDER_FONT_SRC_LIST}
    ${RENDER_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_SRC_LIST}
    ${RENDER_RENDERER_ANIMATION_SRC_LIST}
    ${RENDER_RENDERER_ANIMATION_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_ANIMATION_SHADER_SRC_LIST}
    ${RENDER_RENDERER_DEBUG_SRC_LIST}
    ${RENDER_RENDERER_DEBUG_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_DEBUG_SHADER_SRC_LIST}
    ${RENDER_RENDERER_COMMON_SRC_LIST}
    ${RENDER_RENDERER_COMMON_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_COMMON_SHADER_SRC_LIST}
    ${RENDER_RENDERER_NORMAL_SRC_LIST}
    ${RENDER_RENDERER_NORMAL_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_NORMAL_SHADER_SRC_LIST}
    ${RENDER_RENDERER_FONT_SRC_LIST}
    ${RENDER_RENDERER_FONT_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_FONT_SHADER_SRC_LIST}
    ${RENDER_RENDERER_PARTICLE_SRC_LIST}
    ${RENDER_RENDERER_PARTICLE_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_PARTICLE_SHADER_SRC_LIST}
    ${RENDER_RENDERER_SHADOW_SRC_LIST}
    ${RENDER_RENDERER_SHADOW_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_SHADOW_SHADER_SRC_LIST}
    ${RENDER_RENDERER_SKY_SRC_LIST}
    ${RENDER_RENDERER_SKY_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_SKY_SHADER_SRC_LIST}
    ${RENDER_RENDERER_TERRAIN_SRC_LIST}
    ${RENDER_RENDERER_TERRAIN_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_TERRAIN_SHADER_SRC_LIST}
    ${RENDER_RENDERER_WATER_SRC_LIST}
    ${RENDER_RENDERER_WATER_PIPELINE_SRC_LIST}
    ${RENDER_RENDERER_WATER_SHADER_SRC_LIST}
    ${SCENE_SRC_LIST}
    ${SCENE_LIGHT_SRC_LIST}
    ${SCENE_RAY_CASTING_SRC_LIST}
    ${SCENE_ROBOT_SRC_LIST}
    ${SCENE_SHADOW_SRC_LIST}
    ${SCENE_SKY_SRC_LIST}
    ${SCENE_TERRAIN_SRC_LIST}
    ${SCENE_WATER_SRC_LIST}
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

if(NOT TARGET PreVEngine)
    add_subdirectory("../PreVEngine" "${CMAKE_BINARY_DIR}/PreVEngine")
endif()

target_link_libraries(${PROJECT_NAME} PreVEngine)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Windows/assimp-vc142-mt.lib")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Linux/libassimp.so")
endif()

if(ANDROID)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Android/libassimp.so")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Android/libassimp.so" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/android/app/src/main/libs/arm64-v8a/")
endif()

#  Copy all files from ./Assets to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
