# Shader compilation target for both Vulkan (SPIR-V) and WebGPU (WGSL).
# Uses slangc to compile .slang shaders directly to SPIR-V and WGSL.

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SHADER_MAX_VIEW_COUNT ${MAX_VIEW_COUNT})
set(SHADER_XR_ARGS)
if(ENABLE_XR)
    set(SHADER_XR_ARGS --enable-xr)
endif()

if(ENABLE_XR)
    add_custom_target(CompileShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py --spirv --max-view-count ${SHADER_MAX_VIEW_COUNT} ${SHADER_XR_ARGS}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/spirv"
            "${CMAKE_CURRENT_BINARY_DIR}/spirv"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders (Slang -> SPIR-V)"
        VERBATIM
    )
else()
    add_custom_target(CompileShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py --max-view-count ${SHADER_MAX_VIEW_COUNT}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/spirv"
            "${CMAKE_CURRENT_BINARY_DIR}/spirv"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/wgsl"
            "${CMAKE_CURRENT_BINARY_DIR}/wgsl"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders (Slang -> SPIR-V + WGSL)"
        VERBATIM
    )
endif()
