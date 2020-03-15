project(PreVEngineTest)
cmake_minimum_required(VERSION 2.8)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-std=c++17)
endif()

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("../PreVEngine/External")
include_directories("../Libs/glm")

file(GLOB SRC_LIST "*.h" "*.cpp")  
add_executable(${PROJECT_NAME} ${SRC_LIST})

#---------------Find PreVEngine---------------
if(NOT TARGET PreVEngine)
    add_subdirectory(../PreVEngine ${CMAKE_BINARY_DIR}/PreVEngine)
endif()

target_link_libraries(${PROJECT_NAME} PreVEngine)

if(WIN32)
    include_directories("../Libs/assimp/include")
    target_link_libraries(${PROJECT_NAME} "../../Libs/assimp/lib/Release/assimp-vc142-mt")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories("../Libs/assimp/include")
    target_link_libraries(${PROJECT_NAME} "/home/lubos/Documents/Vulkan/assimp/assimp/build/code/libassimp.so")
endif()

if(ANDROID)
    include_directories("../Libs/assimp_android/include")
    target_link_libraries(${PROJECT_NAME} "/home/lubos/Documents/Vulkan/assimp_android/assimp/contrib/android-cmake/code/libassimp.so")
endif()
#-------------------------------------------

#  Copy all files from ./Assets to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
