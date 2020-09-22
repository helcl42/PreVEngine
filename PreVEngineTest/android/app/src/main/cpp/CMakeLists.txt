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
include_directories(../../../../../../PreVEngine/vk_wrapper)
include_directories(../../../../../../Dependencies/glm)
include_directories(../../../../../../Dependencies/assimp/include)

# Set path to source files.
aux_source_directory(${ANDROID_NDK}/sources/android/native_app_glue     SRC_LIST)

aux_source_directory(../../../../..                                         SRC_LIST)
aux_source_directory(../../../../../render                                  SRC_LIST)
aux_source_directory(../../../../../render/pipeline                         SRC_LIST)
aux_source_directory(../../../../../render/common                           SRC_LIST)
aux_source_directory(../../../../../render/common/shader                    SRC_LIST)
aux_source_directory(../../../../../render/common/pipeline                  SRC_LIST)
aux_source_directory(../../../../../render/debug                            SRC_LIST)
aux_source_directory(../../../../../render/debug/shader                     SRC_LIST)
aux_source_directory(../../../../../render/debug/pipeline                   SRC_LIST)
aux_source_directory(../../../../../render/font                             SRC_LIST)
aux_source_directory(../../../../../render/font/shader                      SRC_LIST)
aux_source_directory(../../../../../render/font/pipeline                    SRC_LIST)
aux_source_directory(../../../../../render/normal                           SRC_LIST)
aux_source_directory(../../../../../render/normal/shader                    SRC_LIST)
aux_source_directory(../../../../../render/normal/pipeline                  SRC_LIST)
aux_source_directory(../../../../../render/particle                         SRC_LIST)
aux_source_directory(../../../../../render/particle/shader                  SRC_LIST)
aux_source_directory(../../../../../render/particle/pipeline                SRC_LIST)
aux_source_directory(../../../../../render/shadow                           SRC_LIST)
aux_source_directory(../../../../../render/shadow/shader                    SRC_LIST)
aux_source_directory(../../../../../render/shadow/pipeline                  SRC_LIST)
aux_source_directory(../../../../../render/sky                              SRC_LIST)
aux_source_directory(../../../../../render/sky/shader                       SRC_LIST)
aux_source_directory(../../../../../render/sky/pipeline                     SRC_LIST)
aux_source_directory(../../../../../render/terrain                          SRC_LIST)
aux_source_directory(../../../../../render/terrain/shader                   SRC_LIST)
aux_source_directory(../../../../../render/terrain/pipeline                 SRC_LIST)
aux_source_directory(../../../../../render/water                            SRC_LIST)
aux_source_directory(../../../../../render/water/shader                     SRC_LIST)
aux_source_directory(../../../../../render/water/pipeline                   SRC_LIST)
aux_source_directory(../../../../../render/animation                        SRC_LIST)
aux_source_directory(../../../../../render/animation/shader                 SRC_LIST)
aux_source_directory(../../../../../render/animation/pipeline               SRC_LIST)

aux_source_directory(../../../../../../PreVEngine/prev                       SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/common                SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/common/pattern        SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/core                  SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/core/device           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/core/instance         SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/core/memory           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/core/memory/buffer    SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/core/memory/image     SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/event                 SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/event/internal        SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/input                 SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/input/keyboard        SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/input/mouse           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/input/touch           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/render                SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/render/image          SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/render/pass           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/render/shader         SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/scene                 SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/scene/component       SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/scene/graph           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/util                  SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/window                SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/window/impl           SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/window/impl/android   SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/window/impl/linux     SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/window/impl/windows   SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/prev/android               SRC_LIST)

aux_source_directory(../../../../../../PreVEngine/vk_wrapper                 SRC_LIST)
aux_source_directory(../../../../../../PreVEngine/external                   SRC_LIST)

# Create native shared library
add_library(native SHARED ${SRC_LIST})

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Dependencies/assimp/lib/Android/libassimp.so" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../libs/arm64-v8a/")

# add external library
add_library(assimp SHARED IMPORTED)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../libs/arm64-v8a/libassimp.so)

# Link to external libraries
target_link_libraries(native log android assimp)

