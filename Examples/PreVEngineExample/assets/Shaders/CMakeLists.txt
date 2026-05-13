# Shader compilation target for both Vulkan (SPIR-V) and WebGPU (WGSL).
# Uses the repository script to preserve folder layout and conversion rules.

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SHADER_MAX_VIEW_COUNT 1)
set(SHADER_XR_ARGS)
if(ENABLE_XR)
    set(SHADER_MAX_VIEW_COUNT 2)
    set(SHADER_XR_ARGS --enable-xr)
endif()

if(ENABLE_XR)
    add_custom_target(CompileShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders_new.py --spirv --max-view-count ${SHADER_MAX_VIEW_COUNT} ${SHADER_XR_ARGS}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/spirv"
            "${CMAKE_CURRENT_BINARY_DIR}/spirv"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders (GLSL -> SPIR-V)"
        VERBATIM
    )
else()
    add_custom_target(CompileShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders_new.py --spirv --max-view-count ${SHADER_MAX_VIEW_COUNT}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders_new.py --wgsl
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/spirv"
            "${CMAKE_CURRENT_BINARY_DIR}/spirv"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/wgsl"
            "${CMAKE_CURRENT_BINARY_DIR}/wgsl"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders (GLSL -> SPIR-V -> WGSL)"
        VERBATIM
    )
endif()
