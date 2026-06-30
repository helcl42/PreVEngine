cmake_minimum_required(VERSION 3.10)

project(PreVEngineTests)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Skip fetching GoogleTest if Dawn already provides it (via BUILD_WEBGPU_BACKEND)
if(NOT TARGET gtest)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        v1.14.0 # Or a specific release tag/commit hash
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()

include_directories("../PreVEngine")
include_directories("../PreVEngine/external")
include_directories("../PreVEngine/external/glm")

set(TEST_SOURCES Main.cpp)

add_executable(PreVEngineTests ${TEST_SOURCES})
target_link_libraries(PreVEngineTests PreVEngine gtest gtest_main)

if(ANDROID)
    target_link_libraries(PreVEngineTests log android)
endif()

# Sanitizers (see top-level CMakeLists). No-op unless ENABLE_ASAN/UBSAN/TSAN is set.
target_link_libraries(PreVEngineTests prev_sanitizers)

add_test(NAME PreVEngineTests COMMAND PreVEngineTests)