project(PreVEngineTest)
cmake_minimum_required(VERSION 2.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-std=c++17)
endif()

include_directories("../Dependencies/glm")
include_directories("../Dependencies/assimp/include")

FILE(GLOB ROOT_SRC_LIST "*.h" "*.cpp")

FILE(GLOB RENDER_SRC_LIST "render/*.h" "render/*.cpp")
FILE(GLOB RENDER_PIPELINE_SRC_LIST "render/pipeline/*.h" "render/pipeline/*.cpp")

FILE(GLOB RENDER_ANIMATION_SRC_LIST "render/animation/*.h" "render/animation/*.cpp")
FILE(GLOB RENDER_ANIMATION_PIPELINE_SRC_LIST "render/animation/pipeline/*.h" "render/animation/pipeline/*.cpp")
FILE(GLOB RENDER_ANIMATION_SHADER_SRC_LIST "render/animation/shader/*.h" "render/animation/shader/*.cpp")

FILE(GLOB RENDER_DEBUG_SRC_LIST "render/debug/*.h" "render/debug/*.cpp")
FILE(GLOB RENDER_DEBUG_PIPELINE_SRC_LIST "render/debug/pipeline/*.h" "render/debug/pipeline/*.cpp")
FILE(GLOB RENDER_DEBUG_SHADER_SRC_LIST "render/debug/shader/*.h" "render/debug/shader/*.cpp")

FILE(GLOB RENDER_COMMON_SRC_LIST "render/common/*.h" "render/common/*.cpp")
FILE(GLOB RENDER_COMMON_PIPELINE_SRC_LIST "render/common/pipeline/*.h" "render/common/pipeline/*.cpp")
FILE(GLOB RENDER_COMMON_SHADER_SRC_LIST "render/common/shader/*.h" "render/common/shader/*.cpp")

FILE(GLOB RENDER_NORMAL_SRC_LIST "render/normal/*.h" "render/normal/*.cpp")
FILE(GLOB RENDER_NORMAL_PIPELINE_SRC_LIST "render/normal/pipeline/*.h" "render/normal/pipeline/*.cpp")
FILE(GLOB RENDER_NORMAL_SHADER_SRC_LIST "render/normal/shader/*.h" "render/normal/shader/*.cpp")

FILE(GLOB RENDER_FONT_SRC_LIST "render/font/*.h" "render/font/*.cpp")
FILE(GLOB RENDER_FONT_PIPELINE_SRC_LIST "render/font/pipeline/*.h" "render/font/pipeline/*.cpp")
FILE(GLOB RENDER_FONT_SHADER_SRC_LIST "render/font/shader/*.h" "render/font/shader/*.cpp")

FILE(GLOB RENDER_PARTICLE_SRC_LIST "render/particle/*.h" "render/particle/*.cpp")
FILE(GLOB RENDER_PARTICLE_PIPELINE_SRC_LIST "render/particle/pipeline/*.h" "render/particle/pipeline/*.cpp")
FILE(GLOB RENDER_PARTICLE_SHADER_SRC_LIST "render/particle/shader/*.h" "render/particle/shader/*.cpp")

FILE(GLOB RENDER_SHADOW_SRC_LIST "render/shadow/*.h" "render/shadow/*.cpp")
FILE(GLOB RENDER_SHADOW_PIPELINE_SRC_LIST "render/shadow/pipeline/*.h" "render/shadow/pipeline/*.cpp")
FILE(GLOB RENDER_SHADOW_SHADER_SRC_LIST "render/shadow/shader/*.h" "render/shadow/shader/*.cpp")

FILE(GLOB RENDER_SKY_SRC_LIST "render/sky/*.h" "render/sky/*.cpp")
FILE(GLOB RENDER_SKY_PIPELINE_SRC_LIST "render/sky/pipeline/*.h" "render/sky/pipeline/*.cpp")
FILE(GLOB RENDER_SKY_SHADER_SRC_LIST "render/sky/shader/*.h" "render/sky/shader/*.cpp")

FILE(GLOB RENDER_TERRAIN_SRC_LIST "render/terrain/*.h" "render/terrain/*.cpp")
FILE(GLOB RENDER_TERRAIN_PIPELINE_SRC_LIST "render/terrain/pipeline/*.h" "render/terrain/pipeline/*.cpp")
FILE(GLOB RENDER_TERRAIN_SHADER_SRC_LIST "render/terrain/shader/*.h" "render/terrain/shader/*.cpp")

FILE(GLOB RENDER_WATER_SRC_LIST "render/water/*.h" "render/water/*.cpp")
FILE(GLOB RENDER_WATER_PIPELINE_SRC_LIST "render/water/pipeline/*.h" "render/water/pipeline/*.cpp")
FILE(GLOB RENDER_WATER_SHADER_SRC_LIST "render/water/shader/*.h" "render/water/shader/*.cpp")

SOURCE_GROUP("" FILES ${ROOT_SRC_LIST})

SOURCE_GROUP("render" FILES ${RENDER_SRC_LIST})
SOURCE_GROUP("render/pipeline" FILES ${RENDER_PIPELINE_SRC_LIST})

SOURCE_GROUP("render/animation" FILES ${RENDER_ANIMATION_SRC_LIST})
SOURCE_GROUP("render/animation/pipeline" FILES ${RENDER_ANIMATION_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/animation/shader" FILES ${RENDER_ANIMATION_SHADER_SRC_LIST})

SOURCE_GROUP("render/debug" FILES ${RENDER_DEBUG_SRC_LIST})
SOURCE_GROUP("render/debug/pipeline" FILES ${RENDER_DEBUG_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/debug/shader" FILES ${RENDER_DEBUG_SHADER_SRC_LIST})

SOURCE_GROUP("render/common" FILES ${RENDER_COMMON_SRC_LIST})
SOURCE_GROUP("render/common/pipeline" FILES ${RENDER_COMMON_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/common/shader" FILES ${RENDER_COMMON_SHADER_SRC_LIST})

SOURCE_GROUP("render/normal" FILES ${RENDER_NORMAL_SRC_LIST})
SOURCE_GROUP("render/normal/pipeline" FILES ${RENDER_NORMAL_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/normal/shader" FILES ${RENDER_NORMAL_SHADER_SRC_LIST})

SOURCE_GROUP("render/font" FILES ${RENDER_FONT_SRC_LIST})
SOURCE_GROUP("render/font/pipeline" FILES ${RENDER_FONT_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/font/shader" FILES ${RENDER_FONT_SHADER_SRC_LIST})

SOURCE_GROUP("render/particle" FILES ${RENDER_PARTICLE_SRC_LIST})
SOURCE_GROUP("render/particle/pipeline" FILES ${RENDER_PARTICLE_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/particle/shader" FILES ${RENDER_PARTICLE_SHADER_SRC_LIST})

SOURCE_GROUP("render/shadow" FILES ${RENDER_SHADOW_SRC_LIST})
SOURCE_GROUP("render/shadow/pipeline" FILES ${RENDER_SHADOW_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/shadow/shader" FILES ${RENDER_SHADOW_SHADER_SRC_LIST})

SOURCE_GROUP("render/sky" FILES ${RENDER_SKY_SRC_LIST})
SOURCE_GROUP("render/sky/pipeline" FILES ${RENDER_SKY_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/sky/shader" FILES ${RENDER_SKY_SHADER_SRC_LIST})

SOURCE_GROUP("render/terrain" FILES ${RENDER_TERRAIN_SRC_LIST})
SOURCE_GROUP("render/terrain/pipeline" FILES ${RENDER_TERRAIN_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/terrain/shader" FILES ${RENDER_TERRAIN_SHADER_SRC_LIST})

SOURCE_GROUP("render/water" FILES ${RENDER_WATER_SRC_LIST})
SOURCE_GROUP("render/water/pipeline" FILES ${RENDER_WATER_PIPELINE_SRC_LIST})
SOURCE_GROUP("render/water/shader" FILES ${RENDER_WATER_SHADER_SRC_LIST})

add_executable(${PROJECT_NAME} 
    ${ROOT_SRC_LIST}
    ${RENDER_SRC_LIST}
    ${RENDER_PIPELINE_SRC_LIST}
    ${RENDER_ANIMATION_SRC_LIST}
    ${RENDER_ANIMATION_PIPELINE_SRC_LIST}
    ${RENDER_ANIMATION_SHADER_SRC_LIST}
    ${RENDER_DEBUG_SRC_LIST}
    ${RENDER_DEBUG_PIPELINE_SRC_LIST}
    ${RENDER_DEBUG_SHADER_SRC_LIST}
    ${RENDER_COMMON_SRC_LIST}
    ${RENDER_COMMON_PIPELINE_SRC_LIST}
    ${RENDER_COMMON_SHADER_SRC_LIST}
    ${RENDER_NORMAL_SRC_LIST}
    ${RENDER_NORMAL_PIPELINE_SRC_LIST}
    ${RENDER_NORMAL_SHADER_SRC_LIST}
    ${RENDER_FONT_SRC_LIST}
    ${RENDER_FONT_PIPELINE_SRC_LIST}
    ${RENDER_FONT_SHADER_SRC_LIST}
    ${RENDER_PARTICLE_SRC_LIST}
    ${RENDER_PARTICLE_PIPELINE_SRC_LIST}
    ${RENDER_PARTICLE_SHADER_SRC_LIST}
    ${RENDER_SHADOW_SRC_LIST}
    ${RENDER_SHADOW_PIPELINE_SRC_LIST}
    ${RENDER_SHADOW_SHADER_SRC_LIST}
    ${RENDER_SKY_SRC_LIST}
    ${RENDER_SKY_PIPELINE_SRC_LIST}
    ${RENDER_SKY_SHADER_SRC_LIST}
    ${RENDER_TERRAIN_SRC_LIST}
    ${RENDER_TERRAIN_PIPELINE_SRC_LIST}
    ${RENDER_TERRAIN_SHADER_SRC_LIST}
    ${RENDER_WATER_SRC_LIST}
    ${RENDER_WATER_PIPELINE_SRC_LIST}
    ${RENDER_WATER_SHADER_SRC_LIST}
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
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Linux/libassimp.so")
endif()

if(ANDROID)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Android/libassimp.so")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/assimp/lib/Android/libassimp.so" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/android/app/src/main/libs/arm64-v8a/")
endif()
#-------------------------------------------

#  Copy all files from ./Assets to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
