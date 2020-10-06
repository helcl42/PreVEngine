project(PreVEngineTest)
cmake_minimum_required(VERSION 2.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

include_directories("../Dependencies/glm")
include_directories("../Dependencies/assimp/include")

FILE(GLOB ROOT_SRC_LIST "*.h" "*.cpp")

FILE(GLOB COMMON_SRC_LIST "common/*.h" "common/*.cpp")

FILE(GLOB COMPONENT_SRC_LIST "component/*.h" "component/*.cpp")
FILE(GLOB COMPONENT_TRANSFORM_SRC_LIST "component/transform/*.h" "component/transform/*.cpp")
FILE(GLOB COMPONENT_TIME_SRC_LIST "component/time/*.h" "component/time/*.cpp")
FILE(GLOB COMPONENT_TERRAIN_SRC_LIST "component/terrain/*.h" "component/terrain/*.cpp")
FILE(GLOB COMPONENT_SKY_SRC_LIST "component/sky/*.h" "component/sky/*.cpp")
FILE(GLOB COMPONENT_RENDER_SRC_LIST "component/render/*.h" "component/render/*.cpp")
FILE(GLOB COMPONENT_LIGHT_SRC_LIST "component/light/*.h" "component/light/*.cpp")
FILE(GLOB COMPONENT_FONT_SRC_LIST "component/font/*.h" "component/font/*.cpp")
FILE(GLOB COMPONENT_CAMERA_SRC_LIST "component/camera/*.h" "component/camera/*.cpp")

FILE(GLOB RENDER_SRC_LIST "render/*.h" "render/*.cpp")
FILE(GLOB RENDER_PIPELINE_SRC_LIST "render/pipeline/*.h" "render/pipeline/*.cpp")

FILE(GLOB RENDER_UTIL_SRC_LIST "render/util/*.h" "render/util/*.cpp")
FILE(GLOB RENDER_UTIL_ASSIMP_SRC_LIST "render/util/assimp/*.h" "render/util/assimp/*.cpp")

FILE(GLOB RENDER_MESH_SRC_LIST "render/mesh/*.h" "render/mesh/*.cpp")
FILE(GLOB RENDER_MODEL_SRC_LIST "render/model/*.h" "render/model/*.cpp")
FILE(GLOB RENDER_MATERIAL_SRC_LIST "render/material/*.h" "render/material/*.cpp")
FILE(GLOB RENDER_ANIMATION_SRC_LIST "render/animation/*.h" "render/animation/*.cpp")

FILE(GLOB RENDER_RENDERER_SRC_LIST "render/renderer/*.h" "render/renderer/*.cpp")

FILE(GLOB RENDER_RENDERER_ANIMATION_SRC_LIST "render/renderer/animation/*.h" "render/renderer/animation/*.cpp")
FILE(GLOB RENDER_RENDERER_ANIMATION_PIPELINE_SRC_LIST "render/renderer/animation/pipeline/*.h" "render/renderer/animation/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_ANIMATION_SHADER_SRC_LIST "render/renderer/animation/shader/*.h" "render/renderer/animation/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_DEBUG_SRC_LIST "render/renderer/debug/*.h" "render/renderer/debug/*.cpp")
FILE(GLOB RENDER_RENDERER_DEBUG_PIPELINE_SRC_LIST "render/renderer/debug/pipeline/*.h" "render/renderer/debug/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_DEBUG_SHADER_SRC_LIST "render/renderer/debug/shader/*.h" "render/renderer/debug/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_COMMON_SRC_LIST "render/renderer/common/*.h" "render/renderer/common/*.cpp")
FILE(GLOB RENDER_RENDERER_COMMON_PIPELINE_SRC_LIST "render/renderer/common/pipeline/*.h" "render/renderer/common/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_COMMON_SHADER_SRC_LIST "render/renderer/common/shader/*.h" "render/renderer/common/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_NORMAL_SRC_LIST "render/renderer/normal/*.h" "render/renderer/normal/*.cpp")
FILE(GLOB RENDER_RENDERER_NORMAL_PIPELINE_SRC_LIST "render/renderer/normal/pipeline/*.h" "render/renderer/normal/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_NORMAL_SHADER_SRC_LIST "render/renderer/normal/shader/*.h" "render/renderer/normal/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_FONT_SRC_LIST "render/renderer/font/*.h" "render/renderer/font/*.cpp")
FILE(GLOB RENDER_RENDERER_FONT_PIPELINE_SRC_LIST "render/renderer/font/pipeline/*.h" "render/renderer/font/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_FONT_SHADER_SRC_LIST "render/renderer/font/shader/*.h" "render/renderer/font/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_PARTICLE_SRC_LIST "render/renderer/particle/*.h" "render/renderer/particle/*.cpp")
FILE(GLOB RENDER_RENDERER_PARTICLE_PIPELINE_SRC_LIST "render/renderer/particle/pipeline/*.h" "render/renderer/particle/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_PARTICLE_SHADER_SRC_LIST "render/renderer/particle/shader/*.h" "render/renderer/particle/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_SHADOW_SRC_LIST "render/renderer/shadow/*.h" "render/renderer/shadow/*.cpp")
FILE(GLOB RENDER_RENDERER_SHADOW_PIPELINE_SRC_LIST "render/renderer/shadow/pipeline/*.h" "render/renderer/shadow/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_SHADOW_SHADER_SRC_LIST "render/renderer/shadow/shader/*.h" "render/renderer/shadow/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_SKY_SRC_LIST "render/renderer/sky/*.h" "render/renderer/sky/*.cpp")
FILE(GLOB RENDER_RENDERER_SKY_PIPELINE_SRC_LIST "render/renderer/sky/pipeline/*.h" "render/renderer/sky/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_SKY_SHADER_SRC_LIST "render/renderer/sky/shader/*.h" "render/renderer/sky/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_TERRAIN_SRC_LIST "render/renderer/terrain/*.h" "render/renderer/terrain/*.cpp")
FILE(GLOB RENDER_RENDERER_TERRAIN_PIPELINE_SRC_LIST "render/renderer/terrain/pipeline/*.h" "render/renderer/terrain/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_TERRAIN_SHADER_SRC_LIST "render/renderer/terrain/shader/*.h" "render/renderer/terrain/shader/*.cpp")

FILE(GLOB RENDER_RENDERER_WATER_SRC_LIST "render/renderer/water/*.h" "render/renderer/water/*.cpp")
FILE(GLOB RENDER_RENDERER_WATER_PIPELINE_SRC_LIST "render/renderer/water/pipeline/*.h" "render/renderer/water/pipeline/*.cpp")
FILE(GLOB RENDER_RENDERER_WATER_SHADER_SRC_LIST "render/renderer/water/shader/*.h" "render/renderer/water/shader/*.cpp")

SOURCE_GROUP("" FILES ${ROOT_SRC_LIST})

SOURCE_GROUP("common" FILES ${COMMON_SRC_LIST})

SOURCE_GROUP("component" FILES ${COMPONENT_SRC_LIST})
SOURCE_GROUP("component/transform" FILES ${COMPONENT_TRANSFORM_SRC_LIST})
SOURCE_GROUP("component/time" FILES ${COMPONENT_TIME_SRC_LIST})
SOURCE_GROUP("component/terrain" FILES ${COMPONENT_TERRAIN_SRC_LIST})
SOURCE_GROUP("component/sky" FILES ${COMPONENT_SKY_SRC_LIST})
SOURCE_GROUP("component/render" FILES ${COMPONENT_RENDER_SRC_LIST})
SOURCE_GROUP("component/light" FILES ${COMPONENT_LIGHT_SRC_LIST})
SOURCE_GROUP("component/font" FILES ${COMPONENT_FONT_SRC_LIST})
SOURCE_GROUP("component/camera" FILES ${COMPONENT_CAMERA_SRC_LIST})

SOURCE_GROUP("render" FILES ${RENDER_SRC_LIST})
SOURCE_GROUP("render/pipeline" FILES ${RENDER_PIPELINE_SRC_LIST})

SOURCE_GROUP("render/util" FILES ${RENDER_UTIL_SRC_LIST})
SOURCE_GROUP("render/util/assimp" FILES ${RENDER_UTIL_ASSIMP_SRC_LIST})

SOURCE_GROUP("render/mesh" FILES ${RENDER_MESH_SRC_LIST})
SOURCE_GROUP("render/model" FILES ${RENDER_MODEL_SRC_LIST})
SOURCE_GROUP("render/material" FILES ${RENDER_MATERIAL_SRC_LIST})
SOURCE_GROUP("render/animation" FILES ${RENDER_ANIMATION_SRC_LIST})

SOURCE_GROUP("render/renderer" FILES ${RENDER_RENDERER_SRC_LIST})

SOURCE_GROUP("render/renderer/animation" FILES ${RENDER_RENDERER_ANIMATION_SRC_LIST})
SOURCE_GROUP("render/renderer/animation/pipeline" FILES ${RENDER_RENDERER_ANIMATION_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/animation/shader" FILES ${RENDER_RENDERER_ANIMATION_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/debug" FILES ${RENDER_RENDERER_DEBUG_SRC_LIST})
SOURCE_GROUP("render/renderer/debug/pipeline" FILES ${RENDER_RENDERER_DEBUG_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/debug/shader" FILES ${RENDER_RENDERER_DEBUG_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/common" FILES ${RENDER_RENDERER_COMMON_SRC_LIST})
SOURCE_GROUP("render/renderer/common/pipeline" FILES ${RENDER_RENDERER_COMMON_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/common/shader" FILES ${RENDER_RENDERER_COMMON_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/normal" FILES ${RENDER_RENDERER_NORMAL_SRC_LIST})
SOURCE_GROUP("render/renderer/normal/pipeline" FILES ${RENDER_RENDERER_NORMAL_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/normal/shader" FILES ${RENDER_RENDERER_NORMAL_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/font" FILES ${RENDER_RENDERER_FONT_SRC_LIST})
SOURCE_GROUP("render/renderer/font/pipeline" FILES ${RENDER_RENDERER_FONT_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/font/shader" FILES ${RENDER_RENDERER_FONT_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/particle" FILES ${RENDER_RENDERER_PARTICLE_SRC_LIST})
SOURCE_GROUP("render/renderer/particle/pipeline" FILES ${RENDER_RENDERER_PARTICLE_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/particle/shader" FILES ${RENDER_RENDERER_PARTICLE_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/shadow" FILES ${RENDER_RENDERER_SHADOW_SRC_LIST})
SOURCE_GROUP("render/renderer/shadow/pipeline" FILES ${RENDER_RENDERER_SHADOW_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/shadow/shader" FILES ${RENDER_RENDERER_SHADOW_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/sky" FILES ${RENDER_RENDERER_SKY_SRC_LIST})
SOURCE_GROUP("render/renderer/sky/pipeline" FILES ${RENDER_RENDERER_SKY_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/sky/shader" FILES ${RENDER_RENDERER_SKY_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/terrain" FILES ${RENDER_RENDERER_TERRAIN_SRC_LIST})
SOURCE_GROUP("render/renderer/terrain/pipeline" FILES ${RENDER_RENDERER_TERRAIN_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/terrain/shader" FILES ${RENDER_RENDERER_TERRAIN_SHADER_SRC_LIST})

SOURCE_GROUP("render/renderer/water" FILES ${RENDER_RENDERER_WATER_SRC_LIST})
SOURCE_GROUP("render/renderer/water/pipeline" FILES ${RENDER_RENDERER_WATER_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/renderer/water/shader" FILES ${RENDER_RENDERER_WATER_SHADER_SRC_LIST})

add_executable(${PROJECT_NAME} 
    ${ROOT_SRC_LIST}
    ${COMMON_SRC_LIST}
    ${COMPONENT_SRC_LIST}
    ${COMPONENT_TRANSFORM_SRC_LIST}
    ${COMPONENT_TIME_SRC_LIST}
    ${COMPONENT_TERRAIN_SRC_LIST}
    ${COMPONENT_SKY_SRC_LIST}
    ${COMPONENT_RENDER_SRC_LIST}
    ${COMPONENT_LIGHT_SRC_LIST}
    ${COMPONENT_FONT_SRC_LIST}
    ${COMPONENT_CAMERA_SRC_LIST}
    ${RENDER_SRC_LIST}
    ${RENDER_UTIL_SRC_LIST}
    ${RENDER_UTIL_ASSIMP_SRC_LIST}
    ${RENDER_MESH_SRC_LIST}
    ${RENDER_MODEL_SRC_LIST}
    ${RENDER_MATERIAL_SRC_LIST}
    ${RENDER_ANIMATION_SRC_LIST}
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
)

#---------------Find PreVEngine---------------
if(NOT TARGET PreVEngine)
    add_subdirectory(../PreVEngine ${CMAKE_BINARY_DIR}/PreVEngine)
endif()

target_link_libraries(${PROJECT_NAME} PreVEngine)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    target_link_libraries(${PROJECT_NAME} "../../Dependencies/assimp/lib/Windows/assimp-vc142-mt")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-std=c++17)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Linux/libassimp.so")
endif()

if(ANDROID)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Android/libassimp.so")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Android/libassimp.so" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/android/app/src/main/libs/arm64-v8a/")
endif()
#-------------------------------------------

#  Copy all files from ./Assets to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
