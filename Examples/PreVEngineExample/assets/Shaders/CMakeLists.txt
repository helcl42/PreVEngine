find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SHADER_MAX_PER_PASS_VIEW_COUNT ${MAX_PER_PASS_VIEW_COUNT})
# Multiview (both eyes in one pass via SV_ViewID) only for OpenXR + ENABLE_MULTIVIEW; else single-view per-eye (WGSL has no SV_ViewID).
set(SHADER_MV_ARGS)
if(MAX_PER_PASS_VIEW_COUNT GREATER 1)
    set(SHADER_MV_ARGS --enable-multiview)
    set(SHADER_XR_MODE "multiview")
else()
    set(SHADER_XR_MODE "per-eye")
endif()

# In place for platforms that package the source tree (iOS/Web/Android); build dir otherwise (keeps the source tree clean).
if(IOS OR EMSCRIPTEN OR ANDROID)
    set(SHADER_OUTPUT_ARGS)
else()
    set(SHADER_OUTPUT_ARGS --output-dir ${CMAKE_CURRENT_BINARY_DIR})
endif()

if(ENABLE_OPENXR)
    add_custom_target(CompileShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py --spirv --shader-dir ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_OUTPUT_ARGS} --max-view-count ${SHADER_MAX_PER_PASS_VIEW_COUNT} ${SHADER_MV_ARGS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders (Slang -> SPIR-V, OpenXR ${SHADER_XR_MODE})"
        VERBATIM
    )
elseif(ENABLE_WEBXR AND EMSCRIPTEN)
    add_custom_target(CompileShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py --wgsl --shader-dir ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_OUTPUT_ARGS} --max-view-count ${SHADER_MAX_PER_PASS_VIEW_COUNT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders (Slang -> WGSL, WebXR per-eye stereo)"
        VERBATIM
    )
else()
    add_custom_target(CompileShaders ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/compile_shaders.py --shader-dir ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_OUTPUT_ARGS} --max-view-count ${SHADER_MAX_PER_PASS_VIEW_COUNT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders (Slang -> SPIR-V + WGSL)"
        VERBATIM
    )
endif()
