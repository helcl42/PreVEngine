# Shader Loader Migration Guide

## Before (Hardcoded Paths)

```cpp
// OLD: Fixed SPIR-V paths, Vulkan only
m_shader = prev::render::shader::ShaderBuilder{ m_device }
    .AddShaderStagePaths({
        { GFX_SHADER_STAGE_VERTEX, "Shaders/sky/sun_occlusion_vert.spv" },
        { GFX_SHADER_STAGE_FRAGMENT, "Shaders/sky/sun_occlusion_frag.spv" }
    })
    // ...
    .Build();
```

## After (Backend-Aware)

```cpp
#include <prev_test/common/ShaderLoader.h>

using namespace prev_test::common;

// NEW: Automatically selects SPIR-V (Vulkan) or WGSL (WebGPU)
m_shader = prev::render::shader::ShaderBuilder{ m_device }
    .AddShaderStagePaths({
        { GFX_SHADER_STAGE_VERTEX, ShaderLoader::GetShaderPath("sky/sun_occlusion_vert", GFX_SHADER_STAGE_VERTEX) },
        { GFX_SHADER_STAGE_FRAGMENT, ShaderLoader::GetShaderPath("sky/sun_occlusion_frag", GFX_SHADER_STAGE_FRAGMENT) }
    })
    // ...
    .Build();
```

## Multiview Support

### Before (Vulkan-only)

```cpp
// Only works with Vulkan
void RenderStereo() {
    // Render with gl_ViewIndex
    for (uint32_t viewIndex = 0; viewIndex < 2; ++viewIndex) {
        // Set view matrix, render
    }
}
```

### After (Vulkan + WebGPU)

```cpp
void RenderStereo() {
    if (ShaderLoader::SupportsNativeMultiview()) {
        // Vulkan: Render once with multiview
        // Shader uses gl_ViewIndex directly
        renderContext.cameraCount = 2;
        // Single render call handles both views
    } else {
        // WebGPU: Render with instancing
        // Shader uses instance_index for view selection
        renderPass.SetInstanceCount(2);
        // Render called twice (once per instance)
    }
}
```

## Shader Conversion Examples

### Multiview Vertex Shader

**GLSL (Vulkan - src/example.vert):**
```glsl
#version 450
#extension GL_EXT_multiview : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 projection[2];  // 2 views
    mat4 view[2];
} ubo;

void main() {
    uint viewIndex = gl_ViewIndex;
    gl_Position = ubo.projection[viewIndex] * ubo.view[viewIndex] * vec4(inPosition, 1.0);
}
```

**WGSL Equivalent (wgsl/example.vert.wgsl - hand-written for WebGPU):**
```wgsl
struct Uniforms {
    projection: array<mat4x4<f32>, 2>,
    view: array<mat4x4<f32>, 2>,
};

@group(0) @binding(0) var<uniform> ubo: Uniforms;

@vertex
fn main(
    @builtin(instance_index) instance_id: u32,
    @location(0) in_pos: vec3<f32>,
) -> @builtin(position) vec4<f32> {
    let view_index = instance_id % 2u;
    return ubo.projection[view_index] * ubo.view[view_index] * vec4<f32>(in_pos, 1.0);
}
```

## Common Issues

### Issue: "glslc not found"
**Solution:** Install VulkanSDK from https://vulkan.lunarg.com/

### Issue: SPIR-V → WGSL conversion produces errors
**Solution:** Hand-write `.wgsl` version in `wgsl/` directory (CMake will skip auto-conversion)

### Issue: Shader file not found
**Solution:** Ensure file is in correct location:
- Source: `assets/Shaders/src/category/name.{vert,frag,comp}`
- Compiled: `assets/Shaders/spirv/category/name.{vert,frag}.spv`
- Converted: `assets/Shaders/wgsl/category/name.{vert,frag}.wgsl`

## Step-by-Step Migration

1. **Create directory structure:**
   ```bash
   mkdir -p assets/Shaders/{src,spirv,wgsl}
   ```

2. **Move GLSL sources (if available):**
   ```bash
   mv assets/Shaders/*.glsl assets/Shaders/src/
   ```

3. **Create ShaderLoader in your code:**
   - Include `ShaderLoader.h`
   - Use `ShaderLoader::GetShaderPath()` for dynamic paths

4. **Build/compile shaders:**
   ```bash
   python Scripts/compile_shaders_new.py
   ```

5. **Add to CMakeLists.txt:**
   ```cmake
   add_subdirectory(Examples/PreVEngineExample/assets/Shaders)
    add_dependencies(PreVEngineExample CompileShaders)
   ```

6. **Update all ShaderBuilder calls** to use `ShaderLoader::GetShaderPath()`

7. **Test both backends** (switch GFX_BACKEND in InstanceFactory.cpp)

## Testing Backend-Specific Code

```cpp
// Check which backend is active
if (ShaderLoader::SupportsNativeMultiview()) {
    std::cout << "Running on Vulkan\n";
} else {
    std::cout << "Running on WebGPU\n";
}
```
