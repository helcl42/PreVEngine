#ifndef __SANDBOX_COMMON_H__
#define __SANDBOX_COMMON_H__

// MAX_VIEW_COUNT_VALUE is propagated by PreVEngine's CMake: 1 for mono, 2 for stereo (XR).
// Fall back to mono so a translation unit still compiles if the definition is missing
// (mirrors the shader's own `#ifndef MAX_VIEW_COUNT` guard).
#ifndef MAX_VIEW_COUNT_VALUE
#define MAX_VIEW_COUNT_VALUE 1
#endif

#endif // !__SANDBOX_COMMON_H__
