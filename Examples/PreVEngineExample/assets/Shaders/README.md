# Shader Architecture: Vulkan + WebGPU Support

## Directory Structure

```
assets/Shaders/
├── src/               # GLSL source files (single source of truth)
│   ├── sky/
│   │   ├── lens_flare.frag
│   │   ├── lens_flare.vert
│   │   ├── sun_occlusion.frag
│   │   └── sun_occlusion.vert
│   └── ...
├── spirv/             # Compiled SPIR-V for Vulkan (auto-generated)
│   ├── sky/
│   │   ├── lens_flare.vert.spv
│   │   ├── lens_flare.frag.spv
│   │   ├── sun_occlusion.vert.spv
│   │   └── sun_occlusion.frag.spv
│   └── ...
├── wgsl/              # WGSL for WebGPU (auto-generated or hand-written)
│   ├── sky/
│   │   ├── lens_flare.vert.wgsl
│   │   ├── lens_flare.frag.wgsl
│   │   ├── sun_occlusion.vert.wgsl
│   │   └── sun_occlusion.frag.wgsl
│   └── ...
└── CMakeLists.txt     # Build rules for compilation
```

## Workflow

### 1. Write Shaders in GLSL

Store all shader source files in `src/` directory. These are the canonical versions:

```glsl
// assets/Shaders/src/sky/lens_flare.vert
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    outTexCoord = inTexCoord;
}
```

### 2. Auto-Compilation

**CMakeLists.txt** automatically:
- Compiles `src/*.glsl` → `spirv/*.spv` using glslc (VulkanSDK)
- Converts `spirv/*.spv` → `wgsl/*.wgsl` using naga (if available)
- Falls back to manual WGSL versions if conversion needed (placed in `wgsl/` directory)

### 3. Load Shaders in Code

Use `ShaderLoader` to get backend-appropriate paths:

```cpp
#include <prev_test/common/ShaderLoader.h>

using namespace prev_test::common;

// OLD (hardcoded SPIR-V):
// "Shaders/sky/lens_flare_vert.spv"

// NEW (backend-aware):
auto vertPath = ShaderLoader::GetShaderPath("sky/lens_flare_vert", GFX_SHADER_STAGE_VERTEX);
// → "Shaders/spirv/sky/lens_flare_vert.spv" (Vulkan)
// → "Shaders/wgsl/sky/lens_flare.vert.wgsl" (WebGPU)
```

## Backend-Specific Features

### Multiview Rendering

**Vulkan (native support):**
```glsl
// Use gl_ViewIndex directly
#version 450
#extension GL_EXT_multiview : enable

layout(location = 0) out vec4 outColor;

void main() {
    uint viewIndex = gl_ViewIndex;  // 0 or 1 for stereo
    // ...
}
```

## Migration Path

1. **Existing SPIR-V shaders**: Stay in `spirv/` directory as-is
2. **Move GLSL sources to `src/`** if you have them
3. **Run CMake build** to auto-compile GLSL → SPIR-V + WGSL
4. **Update shader loaders** to use `ShaderLoader::GetShaderPath()`
5. **Create backend-specific paths** where features differ (multiview, etc.)

## File Naming Convention

- Vertex shader: `name.vert.glsl`, `name.vert.spv`, `name.vert.wgsl`
- Fragment shader: `name.frag.glsl`, `name.frag.spv`, `name.frag.wgsl`  
- Compute shader: `name.comp.glsl`, `name.comp.spv`, `name.comp.wgsl`

## Tools Required

- **glslc** (from VulkanSDK) - GLSL → SPIR-V compilation
- **naga** (optional) - SPIR-V ↔ WGSL conversion
  - Install: `cargo install naga-cli`
  - Or hand-write WGSL versions for critical shaders

## Benefits

✅ Single source of truth (GLSL in `src/`)  
✅ Automatic dual-target compilation  
✅ Backend-aware shader selection  
✅ Easy multiview support (Vulkan native, WebGPU instancing)  
✅ Fallback path for manual WGSL optimization  
