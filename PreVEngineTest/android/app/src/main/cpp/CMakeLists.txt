# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.
cmake_minimum_required(VERSION 3.4.1)

add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
add_definitions(-DUSE_VULKAN_WRAPPER)
add_definitions(-DGLM_FORCE_CXX17)
add_definitions(-DVK_NO_PROTOTYPES)

message("Platform = ${ANDROID_PLATFORM}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

# Set path to header files.
include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(../../../../..)
include_directories(../../../../../../PreVEngine)
include_directories(../../../../../../PreVEngine/external)
include_directories(../../../../../../PreVEngine/android)
include_directories(../../../../../../PreVEngine/vk_wrapper)
include_directories(../../../../../../Dependencies/glm)
include_directories(../../../../../../Dependencies/assimp/include)

# Set path to source files.
aux_source_directory(${ANDROID_NDK}/sources/android/native_app_glue     SRC_LIST)
aux_source_directory(../../../../..                                     SRC_LIST)
aux_source_directory(../../../../../../PreVEngine                       SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/common                SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/common/pattern        SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/core                  SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/core/device           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/core/instance         SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/core/memory           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/core/memory/buffer    SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/core/memory/image     SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/event                 SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/event/internal        SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/input                 SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/input/keyboard        SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/input/mouse           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/input/touch           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/render                SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/render/image          SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/render/pass           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/render/shader         SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/scene                 SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/scene/component       SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/scene/graph           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/util                  SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/window                SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/window/android        SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/window/linux          SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/window/windows        SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/external              SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/android               SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/vk_wrapper            SRC_LIST)

# Create native shared library
add_library(native SHARED ${SRC_LIST})

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Dependencies/assimp/lib/Android/libassimp.so" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../libs/arm64-v8a/")

# add external library
add_library(assimp SHARED IMPORTED)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../libs/arm64-v8a/libassimp.so)

# Link to external libraries
target_link_libraries(native log android assimp)

