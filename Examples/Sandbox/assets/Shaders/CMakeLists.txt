# Compiles this example's Slang shaders via slangc.
#   Mono: SPIR-V (Vulkan) + WGSL (WebGPU).
#   XR:   SPIR-V only, with multiview (SV_ViewID) enabled. XR is Vulkan-only.
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SANDBOX_SHADER_MAX_VIEW_COUNT ${MAX_VIEW_COUNT})

if(ENABLE_XR)
    add_custom_target(CompileSandboxShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py
                --shader-dir ${CMAKE_CURRENT_SOURCE_DIR}
                --spirv
                --enable-xr
                --max-view-count ${SANDBOX_SHADER_MAX_VIEW_COUNT}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/spirv"
            "${CMAKE_CURRENT_BINARY_DIR}/spirv"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling Sandbox shaders (Slang -> SPIR-V, XR multiview)"
        VERBATIM
    )
else()
    add_custom_target(CompileSandboxShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py
                --shader-dir ${CMAKE_CURRENT_SOURCE_DIR}
                --max-view-count ${SANDBOX_SHADER_MAX_VIEW_COUNT}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/spirv"
            "${CMAKE_CURRENT_BINARY_DIR}/spirv"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/wgsl"
            "${CMAKE_CURRENT_BINARY_DIR}/wgsl"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling Sandbox shaders (Slang -> SPIR-V + WGSL)"
        VERBATIM
    )
endif()
