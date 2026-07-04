find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SANDBOX_SHADER_MAX_PER_PASS_VIEW_COUNT ${MAX_PER_PASS_VIEW_COUNT})
# Multiview (both eyes in one pass via SV_ViewID) only for OpenXR + ENABLE_MULTIVIEW; else single-view per-eye.
set(SANDBOX_SHADER_MV_ARGS)
if(MAX_PER_PASS_VIEW_COUNT GREATER 1)
    set(SANDBOX_SHADER_MV_ARGS --enable-multiview)
    set(SANDBOX_SHADER_XR_MODE "multiview")
else()
    set(SANDBOX_SHADER_XR_MODE "per-eye")
endif()

if(ENABLE_OPENXR)
    set(SANDBOX_SHADER_MODE_ARGS --spirv ${SANDBOX_SHADER_MV_ARGS})
    set(SANDBOX_SHADER_MODE_DESC "SPIR-V, OpenXR ${SANDBOX_SHADER_XR_MODE}")
elseif(ENABLE_WEBXR AND EMSCRIPTEN)
    set(SANDBOX_SHADER_MODE_ARGS --wgsl)
    set(SANDBOX_SHADER_MODE_DESC "WGSL, WebXR per-eye stereo")
else()
    set(SANDBOX_SHADER_MODE_ARGS)
    set(SANDBOX_SHADER_MODE_DESC "SPIR-V + WGSL")
endif()

set(SANDBOX_SHADER_SCRIPT_ARGS ${SANDBOX_SHADER_MODE_ARGS} --shader-dir ${CMAKE_CURRENT_SOURCE_DIR} --max-view-count ${SANDBOX_SHADER_MAX_PER_PASS_VIEW_COUNT})

if(IOS OR EMSCRIPTEN OR ANDROID)
    # In-place platforms: compile at configure time so shaders exist before packaging. Reconfigure to recompile.
    message(STATUS "Compiling Sandbox shaders (${SANDBOX_SHADER_MODE_DESC})")
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py ${SANDBOX_SHADER_SCRIPT_ARGS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE SANDBOX_SHADER_COMPILE_RESULT
    )
    if(NOT SANDBOX_SHADER_COMPILE_RESULT EQUAL 0)
        message(FATAL_ERROR "Sandbox shader compilation failed (${SANDBOX_SHADER_MODE_DESC})")
    endif()
else()
    # Desktop: build-time target, incremental recompiles into the build dir.
    add_custom_target(CompileSandboxShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py ${SANDBOX_SHADER_SCRIPT_ARGS} --output-dir ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling Sandbox shaders (${SANDBOX_SHADER_MODE_DESC})"
        VERBATIM
    )
endif()
