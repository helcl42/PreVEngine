project(PreVEngineTest)
cmake_minimum_required(VERSION 2.8)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-std=c++17)
endif()

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("../PreVEngine/External")
include_directories("../Libs/glm")
include_directories("../Libs/assimp/include")

file(GLOB SRC_LIST "*.h" "*.cpp")  
add_executable(${PROJECT_NAME} ${SRC_LIST})

#---------------Find PreVEngine---------------
if(NOT TARGET PreVEngine)
    add_subdirectory(../PreVEngine ${CMAKE_BINARY_DIR}/PreVEngine)
endif()

target_link_libraries(${PROJECT_NAME} PreVEngine)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} "../../Libs/assimp/lib/Release/assimp-vc142-mt")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # build assimp for LInux
endif()

if(ANDROID)
    # build assimp for Android
endif()
#-------------------------------------------

#  Copy all files from ./Assets to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
