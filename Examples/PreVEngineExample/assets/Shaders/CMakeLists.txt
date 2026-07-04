find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SHADER_MAX_PER_PASS_VIEW_COUNT ${MAX_PER_PASS_VIEW_COUNT})
# Multiview (both eyes in one pass via SV_ViewID) only for OpenXR + ENABLE_MULTIVIEW; else single-view per-eye.
set(SHADER_MV_ARGS)
if(MAX_PER_PASS_VIEW_COUNT GREATER 1)
    set(SHADER_MV_ARGS --enable-multiview)
    set(SHADER_XR_MODE "multiview")
else()
    set(SHADER_XR_MODE "per-eye")
endif()

if(ENABLE_OPENXR)
    set(SHADER_MODE_ARGS --spirv ${SHADER_MV_ARGS})
    set(SHADER_MODE_DESC "SPIR-V, OpenXR ${SHADER_XR_MODE}")
elseif(ENABLE_WEBXR AND EMSCRIPTEN)
    set(SHADER_MODE_ARGS --wgsl)
    set(SHADER_MODE_DESC "WGSL, WebXR per-eye stereo")
else()
    set(SHADER_MODE_ARGS)
    set(SHADER_MODE_DESC "SPIR-V + WGSL")
endif()

set(SHADER_SCRIPT_ARGS ${SHADER_MODE_ARGS} --shader-dir ${CMAKE_CURRENT_SOURCE_DIR} --max-view-count ${SHADER_MAX_PER_PASS_VIEW_COUNT})

if(IOS OR EMSCRIPTEN OR ANDROID)
    # In-place platforms: compile at configure time so shaders exist before packaging. Reconfigure to recompile.
    message(STATUS "Compiling shaders (${SHADER_MODE_DESC})")
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py ${SHADER_SCRIPT_ARGS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE SHADER_COMPILE_RESULT
    )
    if(NOT SHADER_COMPILE_RESULT EQUAL 0)
        message(FATAL_ERROR "Shader compilation failed (${SHADER_MODE_DESC})")
    endif()
else()
    # Desktop: build-time target, incremental recompiles into the build dir.
    add_custom_target(CompileShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py ${SHADER_SCRIPT_ARGS} --output-dir ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders (${SHADER_MODE_DESC})"
        VERBATIM
    )
endif()
