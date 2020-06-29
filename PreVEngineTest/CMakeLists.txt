project(PreVEngineTest)
cmake_minimum_required(VERSION 2.8)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-std=c++17)
endif()

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("../PreVEngine/External")
include_directories("../Dependencies/glm")
include_directories("../Dependencies/assimp/include")

file(GLOB SRC_LIST "*.h" "*.cpp")  
add_executable(${PROJECT_NAME} ${SRC_LIST})

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
    target_link_libraries(${PROJECT_NAME} "/home/lubos/Documents/Vulkan/assimp/assimp/build/code/libassimp.so")
endif()

if(ANDROID)
    target_link_libraries(${PROJECT_NAME} "/home/lubos/Documents/Vulkan/assimp_android/assimp/contrib/android-cmake/code/libassimp.so")
endif()
#-------------------------------------------

#  Copy all files from ./Assets to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
